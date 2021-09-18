/*
  Written by Rami Räsänen <raipsu@users.sourceforge.net>

  This file is part of PS2VIC.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
  02111-1307  USA.
*/

#include <tamtypes.h>
#include "bitmap.h"
#include "libpad.h"
#include "pad.h"
#include "g2.h"

#define WINX (disp_w/2-WINW/2)
#define WINY (disp_h/2-WINH/2)
#define WINH 22

extern FONT f_arial;
extern FONT f_retaliator;

void show_msg (char *msg) {
#define WINW winw
  int winw;

  winw = text_len(&f_arial, msg) + 10;

//  vic_i_draw_curframe();
  g2_set_fill_color(90, 20, 20);
  g2_fill_rect(WINX, WINY, WINX+WINW, WINY+WINH);
  g2_set_fill_color(0, 0, 0);
  render_text_c (&f_arial, WINX, WINY+WINH/2-f_arial.h/2, WINW, msg, 0xF0F0F0);

  g2_set_fill_color(0, 0, 0);

  g2_flip_buffers();
}

void draw_msg (char *msg) {
#define WINW winw
  int winw;

  winw = text_len(&f_arial, msg) + 10;

  g2_set_fill_color(90, 20, 20);
  g2_fill_rect(WINX, WINY, WINX+WINW, WINY+WINH);
  g2_set_fill_color(0, 0, 0);
  render_text_c (&f_arial, WINX, WINY+WINH/2-f_arial.h/2, WINW, msg, 0xF0F0F0);

  g2_set_fill_color(0, 0, 0);
}

void show_ack (char *msg) {
#undef WINW
#undef WINH
#define WINW winw
#define WINH 50
  int l, l2;
  int winw;
  g2_flip_buffers();
  l = text_len(&f_arial, "Press X");
  l2 = text_len(&f_arial, msg);
  if (l2 > l)
    l = l2;
  winw = l + 10;
  g2_set_fill_color(20, 20, 40);
  g2_fill_rect(WINX, WINY, WINX+WINW, WINY+WINH);
  g2_set_fill_color(0, 0, 0);
  render_text_c (&f_arial, WINX, WINY-2, WINW, msg, 0xF0F0F0);
  render_text_c (&f_arial, WINX, WINY+WINH-21, WINW, "Press X", 0x90F090);

  g2_set_fill_color(0, 0, 0);
  g2_flip_buffers();

  for (;;) {
    g2_wait_vsync();
    pad_update();
    if (new_pad & PAD_CROSS)
      break;
  }

}

