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

#include "vice.h"

#include "clkguard.h"
#include "cmdline.h"
#include "drive.h"
#include "libpad.h"
#include "machine.h"
#include "maincpu.h"
#include "resources.h"
#include "videoarch.h"
#include "sound.h"
#include "ui.h"
#include "types.h"
#include "vsync.h"
#include "gs.h"
#include "g2.h"
#include "vkeyboard.h"
#include "kbd.h"
#include "joy.h"
#include "menu.h"
#include "fileexp.h"
#include "infowin.h"
#include "cdvd_vsync.h"
#include "pad.h"

extern unsigned char *ps2_videobuf;

/* Maximum number of frames we can skip consecutively when adjusting the
   refresh rate dynamically.  */
#define MAX_SKIPPED_FRAMES	10

/* ------------------------------------------------------------------------- */

/* Vsync-related resources.  */
static const resource_t resources[] = {
    { NULL }
};

int vsync_resources_init(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

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

int vsync_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

/* Number of frames per second on the real machine.  */
static double refresh_frequency;

/* Number of clock cycles per seconds on the real machine.  */
static long cycles_per_sec;

/* Function to call at the end of every screen frame.  */
static void (*vsync_hook)(void);

/* ------------------------------------------------------------------------- */

static int timer_speed = -1;

int ps2_framecount = 0;

/* Speed evaluation. */

static CLOCK speed_eval_prev_clk;

static int speed_eval_suspended = 1;

void vsync_suspend_speed_eval(void)
{
    speed_eval_suspended = 1;
}

void vsync_sync_reset(void)
{
    speed_eval_suspended = 1;
}

static void calc_avg_performance(int num_frames)
{
}

double vsync_get_avg_frame_rate(void)
{
  return 0;
}

double vsync_get_avg_speed_index(void)
{
  return 0;
}

/* ------------------------------------------------------------------------- */

/* This prevents the clock counters from overflowing.  */
static void clk_overflow_callback(CLOCK amount, void *data)
{
    speed_eval_prev_clk -= amount;
}

/* ------------------------------------------------------------------------- */

int is_paused = 0;

int vsync_do_vsync(struct video_canvas_s *c, int been_skipped) {
  vsync_hook();

  ps2_framecount++;

  do {
    kbd_update();
    pad_update();

    g2_fill_rect(0, 0, disp_w, disp_h);
    g2_put_image((disp_w-c->width)/2, (disp_h-c->height)/2, c->width, c->height, c->width, c->height, (u32*)ps2_videobuf);

    if (is_paused) {
      draw_msg("PAUSED");
      SjPCM_Clearbuff();
    }

    if (aboutwin_active) {
      aboutwin_frame();
    } else if (infowin_active) {
      infowin_frame();
    } else if (file_explorer_active) {
      file_explorer_frame();
    } else if (!menu_update()) {
      vkeyboard_frame();
      infomsg_frame();
      if (new_pad1 || new_rel1) {
        joy_setabs();
      }
      if (new_pad2 || new_rel2) {
        joy2_setabs();
      }
    }

    cdvd_vsync();

    g2_flip_buffers();
  } while (is_paused);

  sound_flush(100);
  kbd_buf_flush();
  return 0;
}

/* -------------------------------------------------------------------------- */

void vsync_set_machine_parameter(double refresh_rate, long cycles)
{
    refresh_frequency = refresh_rate;
    cycles_per_sec = cycles;
}

void vsync_init(void (*hook)(void))
{
  vsync_hook = hook;
}

int vsync_disable_timer(void)
{
    /* FIXME: Find a more generic solution.  */
    if (machine_class == VICE_MACHINE_CBM2)
        timer_speed = 0;
    return 0;
}

void vsyncarch_presync() {

}


void vsyncarch_display_speed(double speed, double frame_rate, int warp_enabled) {

}

unsigned long vsyncarch_gettime(void) {
  return 0;
}
