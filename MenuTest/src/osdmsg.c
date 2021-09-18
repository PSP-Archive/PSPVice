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

#include "types.h"
#include "bitmap.h"
#ifdef WIN32
#include "Win32pad.h"
#else
#include "pad.h"
#endif


#define NULL 0 

extern int disp_w;
extern int disp_h;

#define WINX (disp_w/2-WINW/2)
#define WINY (disp_h/2-WINH/2)
#define WINH 7+8+7 // Line is 8 pixels. 7 pixels before

void show_msg (char *msg)
{
#define WINW winw
  
  int winw;

  winw = text_len(NULL, msg) + 10;

  PSPSetFillColor(90, 20, 20,192);
  PSPFillRectangle(WINX, WINY, WINX+WINW, WINY+WINH);
  render_text_c (NULL, WINX, WINY+7, WINW, msg, 0xF0F0F0);
}

void draw_msg (char *msg)
{
#define WINW winw
  int winw;

  winw = text_len(NULL, msg) + 10;

  PSPSetFillColor(90, 20, 20,192);
  PSPFillRectangle(WINX, WINY, WINX+WINW, WINY+WINH);
  render_text_c (NULL, WINX, WINY+WINH/2-8/2, WINW, msg, 0xF0F0F0);
}

void show_ack (char *msg)
{
#undef WINW
#undef WINH
#define WINW winw
#define WINH (7+8+7+8+7)
  int l, l2;
  int winw;
  l = text_len(NULL, "Press X");
  l2 = text_len(NULL, msg);
  if (l2 > l)
    l = l2;
  winw = l + 10;
  PSPSetFillColor(20, 20, 40,192);
  PSPFillRectangle(WINX, WINY, WINX+WINW, WINY+WINH);
  render_text_c (NULL, WINX, WINY+7, WINW, msg, 0xF0F0F0);
  render_text_c (NULL, WINX, WINY+7+8+7, WINW, "Press X", 0x90F090);

  for (;;)
  {
    pad_update();
    if (new_pad & PAD_CROSS)
      break;
  }
}

