/*
 * vsync.c - End-of-frame handling for MS-DOS.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

//void pspaudiologframe(float fps);

#include "vice.h"

#include "clkguard.h"
#include "cmdline.h"
#include "drive.h"
#include "machine.h"
#include "maincpu.h"
#include "resources.h"
#include "videoarch.h"
#include "sound.h"
#include "ui.h"
#include "types.h"
#include "vsync.h"
#include "vkeyboard.h"
#include "kbd.h"
#include "joy.h"
#include "menu.h"
#include "fileexp.h"
#include "infowin.h"
#include "pad.h"
#include "osdmsg.h"
#include "joy.h"
#include "kbdbuf.h"

#include "sys/time.h"

#include "PSPSpecific.h"
#include "PSPInputs.h"
#include "pspaudio.h"

extern int in_gameloop;
void DisplayDebugMessages();

extern unsigned char *ps2_videobuf;

/* Maximum number of frames we can skip consecutively when adjusting the
   refresh rate dynamically.  */
#define MAX_SKIPPED_FRAMES	10

/* ------------------------------------------------------------------------- */

/* Vsync-related resources.  */
static const resource_int_t resources[] = {
    { NULL }
};

int vsyncarch_resources_init(void)
{
    return resources_register_int(resources);
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

/* Vsync-related command-line options.  */
static const cmdline_option_t cmdline_options[] = {
    { "-speed", SET_RESOURCE, 1, NULL, NULL, "Speed", NULL,
      "<percent>", "Limit emulation speed to specified value" },
    { "-refresh", SET_RESOURCE, 1, NULL, NULL, "RefreshRate", NULL,
      "<value>", "Update every <value> frames (`0' for automatic)" },
    { "-warp", SET_RESOURCE, 0, NULL, NULL, "WarpMode", (resource_value_t)1,
      NULL, "Enable warp mode" },
    { "+warp", SET_RESOURCE, 0, NULL, NULL, "WarpMode", (resource_value_t)0,
      NULL, "Disable warp mode" },
    { NULL }
};

int vsyncarch_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

/* Number of frames per second on the real machine.  */
static float refresh_frequency;

/* Number of clock cycles per seconds on the real machine.  */
static long cycles_per_sec;

/* Function to call at the end of every screen frame.  */
static void (*vsyncarch_hook)(void);

/* ------------------------------------------------------------------------- */

static int timer_speed = -1;

//int ps2_framecount = 0;

/* Speed evaluation. */

static CLOCK speed_eval_prev_clk;

static int speed_eval_suspended = 1;

void vsyncarch_suspend_speed_eval(void)
{
    speed_eval_suspended = 1;
}

void vsyncarch_sync_reset(void)
{
    speed_eval_suspended = 1;
}

static void calc_avg_performance(int num_frames)
{
}

float vsyncarch_get_avg_frame_rate(void)
{
  return 0;
}

float vsyncarch_get_avg_speed_index(void)
{
  return 0;
}

/* ------------------------------------------------------------------------- */

/* This prevents the clock counters from overflowing.  */
static void clk_overflow_callback(CLOCK amount, void *data)
{
    speed_eval_prev_clk -= amount;
}

int disp_w = 480;
int disp_h = 272; 

// -------------------------------------------------------------------------

int is_paused = 0;

float lastvalue;
float elaspsed_time;
float cumulateddelaytime=0.0f; // Count how much time we are ahead/above from real emulation

#define PALFRAMETIME  ((1/50.0f)*1000.0f)
#define NTSCFRAMETIME ((1/60.0f)*1000.0f)

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
int wait_1ms()
{
  int i;
  int a;
  float b;
  a=0;
  b=0.0f;
  for (i=0; i<100000; i++)
  {
    a=i;
    b=b*a;
    b=b/a;
  }
  return b;
}

struct video_canvas_s* stored_canvas; // For menu
  
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
int vsyncarch_do_vsync(struct video_canvas_s* c, int been_skipped)
{

  stored_canvas = c;

  // -- Get Time
	
	struct timeval tval;
	float curr;

  do
  {
    elaspsed_time = 16.6666f;
    lastvalue = curr;
    
    if (elaspsed_time>2*NTSCFRAMETIME)
      elaspsed_time=NTSCFRAMETIME-1.0f;
    else if (elaspsed_time<0)
      elaspsed_time=1.0f;
    
    in_gameloop=1;

    PSPDisplayImage_Reset();

    PSPStartDisplay();

    kbd_update();
    pad_update();
    PSPInputs_Update(); // Set the PSPVice pressed keys, via key mapping

    PSPDisplayVideoBuffer();

    // Draw message "Pause"
    if ( (is_paused) && (menu_active==0) )
    {
      draw_msg("PAUSED");
      //pspaudio_clear();
    }

    if (aboutwin_active)
    {
      aboutwin_frame();
    } 
    else if (infowin_active)
    {
      infowin_frame();
    }
    else if (file_explorer_active)
    {
      file_explorer_frame();
    }
    else if (!menu_update())
    {
      vkeyboard_frame();
      infomsg_frame();
      
      joy_setabs();
      joy2_setabs();
      
      if ( !vkeyboard_IsActive() )
      {
        // Test special buttons (if some keys have been remapped on PSP buttons, like Square=Space and Circle=Enter
        unsigned char currentPSPViceKeyMapped;
        int i;
        // Test all PSP buttons, if some have C64 keys then say to emu that key is pressed
        for (i=0; i<12; i++) // 12 PSP buttons
        {
          // Get name of key mapped on "Square" PSP Button
          currentPSPViceKeyMapped = PSPInputs_GetMappingValue(i);
          // If this is a keyboard key, then use it here
          if ( currentPSPViceKeyMapped < 100 )
          {
            if ( PSPInputs_IsButtonJustPressed( currentPSPViceKeyMapped ) )
            {
               kbd_press_button(currentPSPViceKeyMapped);
            }
            else if ( PSPInputs_IsButtonJustReleased( currentPSPViceKeyMapped ) )
            {
              kbd_rel_button(currentPSPViceKeyMapped);
            }
          }
        }    
      }      
      
      // Test special functions
      if ( PSPInputs_IsButtonJustReleased( pspinp_SwapJoysticks ) )
      {
        swap_joysticks = !swap_joysticks;
      }
      
    }

    if (menu_active)
    {
      scrolling_update();
      scrolling_display();
    }

    DisplayDebugMessages(); // Debug (printf display on screen)
    
    // Display Drive Status
    extern int                  ui_status_led[2];
    extern float                ui_status_track[2];    
    char message[256];
    if ( ui_status_led[0] )
    {
      sprintf(message, "Drive Track : %2.1f", (ui_status_track[0]/2.0f));
      PSPDisplayText(480-(20*8),  16, 0x0ffffff, message); 
    }

    // Debug. Take a real PSP screenshot (for documentation)
    /*
    extern void TakeScreenshot();
    if ( new_rel & PSP_CTRL_RTRIGGER )
      TakeScreenshot();
    */
    
    PSPEndDisplay();

		if (is_paused)
		{
   		PSPWaitVBL();
		}
		else
		{
    }

		PSPSwap(); // Swap screen

    if (is_paused)
      pspaudio_clear();	

  } while (is_paused);

  sound_flush(100);

  kbdbuf_flush();

  return 0;
}

/* -------------------------------------------------------------------------- */

void vsyncarch_set_machine_parameter(float refresh_rate, long cycles)
{
    refresh_frequency = refresh_rate;
    cycles_per_sec = cycles;
}

void vsyncarch_init()
{
	struct timeval tval;
	float curr;

  cumulateddelaytime=0.0f;
}

int vsyncarch_disable_timer(void)
{
    /* FIXME: Find a more generic solution.  */
    if (machine_class == VICE_MACHINE_CBM2)
        timer_speed = 0;
    return 0;
}

void vsyncarch_presync()
{

}


void vsyncarch_display_speed(float speed, float frame_rate, int warp_enabled) {

}

unsigned long vsyncarch_gettime(void)
{
    struct timeval now;

    gettimeofday(&now, NULL);

    return 1000000UL*now.tv_sec + now.tv_usec; 
}

/* Number of timer units per second. */
signed long vsyncarch_frequency(void)
{
    /* Microseconds resolution. */
    return 1000000;
} 

/* Sleep a number of timer units. */
void vsyncarch_sleep(signed long delay)
{
    //snooze(delay);
    sceKernelDelayThread(delay);
} 

void vsyncarch_postsync(void)
{
	
} 


/* Set the screen refresh rate, as this is variable in the CRTC */
void machine_set_cycles_per_frame(long cpf)
{
  /* sound_set_cycles_per_rfsh(machine_timing.cycles_per_rfsh); */
} 