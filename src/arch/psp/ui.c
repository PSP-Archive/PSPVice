/*
 * ui.c - A (very) simple user interface for MS-DOS.
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

#include "vice.h"

#include <fcntl.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

#include "cmdline.h"
#include "datasette.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "menudefs.h"
#include "monitor.h"
#include "resources.h"
#include "sound.h"
#include "types.h"
#include "ui.h"
#include "video.h"
#include "videoarch.h"
#include "version.h"
#include "vsync.h"
#include "infowin.h"

/* Status of keyboard LEDs.  */
static int kbd_led_status;

#define DRIVE0_LED_MSK 4
#define DRIVE1_LED_MSK 4
#define WARP_LED_MSK 1

static const resource_int_t resources[] = {
    { NULL }
};


int ui_resources_init(void)
{
    return resources_register_int(resources);
}

void ui_resources_shutdown(void)
{
}

int ui_cmdline_options_init(void)
{
  return 0;
}

static void ui_exit(void)
{
}

int ui_init(int *argc, char **argv)
{
    return 0;
}

void ui_shutdown(void)
{

}

int ui_init_finish(void)
{
    return 0;
}

int ui_init_finalize(void)
{
    return 0;
}

extern float vsync_get_avg_frame_rate(void);
extern float vsync_get_avg_speed_index(void);

void ui_main(char hotkey)
{
}

void ui_error(const char *format,...)
{
/*
    char buf[512];
    char *tmp;
    va_list ap;

    va_start(ap, format);
    tmp = lib_mvsprintf(format, ap);
    printf ("ERROR: %s\n", tmp);
    snprintf (buf, sizeof(buf), "%s", tmp);

    show_infowin_nonblock(buf);

    lib_free(tmp);
*/
    // CK
    char localstring[256];
    va_list ap;
    va_start(ap, format);
    vsprintf(localstring, format, ap);
    printf(localstring);
    return;    
}

ui_jam_action_t ui_jam_dialog(const char *format,...)
{
    char *tmp;
    char buf[512];
    va_list ap;

    va_start(ap, format);
    tmp = lib_mvsprintf(format, ap);
    printf ("JAM: %s\n", tmp);
    snprintf (buf, sizeof(buf), "JAM: %s", tmp);

    show_infowin_nonblock(buf);

    lib_free(tmp);

    /* Always hard reset.  */
    return UI_JAM_HARD_RESET;
}

void ui_show_text(const char *title, const char *text)
{
}

void ui_update_menus(void)
{
}

ui_drive_enable_t    ui_drive_enabled;
int                  ui_status_led[2];
float                ui_status_track[2];
int                 *ui_drive_active_led;

static void ui_draw_drive_status(int drive_bar)
{
}


void ui_enable_drive_status(ui_drive_enable_t state, int *drive_led_color)
{
    if (!(state & UI_DRIVE_ENABLE_0))
        ui_display_drive_led(0, 0, 0);
    if (!(state & UI_DRIVE_ENABLE_1))
        ui_display_drive_led(1, 0, 0);
    ui_drive_enabled = state;
    ui_drive_active_led = drive_led_color;
    ui_draw_drive_status(0);
    ui_draw_drive_status(1);
}

/* drive_base is either 8 or 0 depending on unit or drive display.
   Dual drives display drive 0: and 1: instead of unit 8: and 9: */
void ui_display_drive_track(unsigned int drive_number, unsigned int drive_base, 
                            unsigned int half_track_number)
{
    float track_number = (float)half_track_number;

    ui_status_track[drive_number] = track_number;
    ui_draw_drive_status(drive_number);

}

void ui_display_drive_led(int drive_number, unsigned int status, unsigned int led_pwm2)
{
    switch (drive_number) {
      case 0:
        if (status)
            kbd_led_status |= DRIVE0_LED_MSK;
        else
            kbd_led_status &= ~DRIVE0_LED_MSK;
        break;
      case 1:
        if (status)
            kbd_led_status |= DRIVE1_LED_MSK;
        else
            kbd_led_status &= ~DRIVE1_LED_MSK;
        break;
      default:
        break;
    }
    ui_status_led[drive_number]=status;
    ui_draw_drive_status(drive_number);
}

/* display current image */
void ui_display_drive_current_image(unsigned int drivenum, const char *image)
{
    /* just a dummy so far */
}


/* tape-related ui */

static int ui_tape_enabled = 0;
static int ui_tape_counter = -1;
static int ui_tape_motor = -1;
static int ui_tape_control = -1;

static void ui_draw_tape_status()
{
}


void ui_set_tape_status(int tape_status)
{
    ui_tape_enabled = tape_status;
    ui_draw_tape_status();
}

void ui_display_tape_motor_status(int motor)
{
    ui_tape_motor = motor;
    ui_draw_tape_status();
}

void ui_display_tape_control_status(int control)
{
    ui_tape_control = control;
    ui_draw_tape_status();
}


void ui_display_tape_counter(int counter)
{   
    if (counter == ui_tape_counter)
        return;

    ui_tape_counter = counter;
    ui_draw_tape_status();
}


void ui_display_tape_current_image(const char *image)
{
}

void ui_display_recording(int recording_status)
{
}

void ui_display_playback(int playback_status, char *version)
{
}


void ui_set_warp_status(int status)
{
    if (status)
        kbd_led_status |= WARP_LED_MSK;
    else
        kbd_led_status &= ~WARP_LED_MSK;
}

int ui_extend_image_dialog(void)
{
    int ret;

    printf ("Extend disk image in drive 8 to 40 tracks?  (Y/N) N\n");
    return 0;

    return ret;
}

void ui_display_speed(float percent, float framerate, int warp_flag)
{
}



/* ------------------------------------------------------------------------- */

void ui_dispatch_events(void)
{
}

void ui_display_event_time(unsigned int current, unsigned int total)
{
} 

/* ------------------------------------------------------------------------- */
/* Dispay the joystick status.  */
void ui_display_joyport(BYTE *joyport)
{
}

void ui_display_volume(int vol)
{
} 
 
