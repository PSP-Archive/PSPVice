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

#include <stdio.h>
#include <tamtypes.h>
#include <string.h>
#include "libpad.h"
#include "pad.h"
#include "vkeyboard.h"
#include "bitmap.h"
#include "vic20.h"
#include "via.h"
#include "kbd.h"
#include "g2.h"

#define B1_X 10
#define B2_X (disp_w-10)
#define B1_Y 5
#define B2_Y (disp_h-5)

unsigned char pressed_keys[256] = {0, 0};
unsigned char joystick_buttons[5] = {0};

/*

       normaali nappi:  15 x 11
   ctrl+shift+restore:  24 x 11
               return:  33 x 11
                space: 160 x 11

   vaakaväli: 3
   pystyväli: 1

   nuolivasemmalle ja ctrl x start: 15
         stop ja commodore x start: 10
                     space x start: 59

   fx x start: 311

   rivi 1: 9 - 19
    col 1: 15 - 29 (14)

   rivi 2: 21 - 31
   rivi 3: 33 - 43
   rivi 4: 45 - 55

*/

extern BITMAP gr_pointer;
extern BITMAP gr_button1;
extern BITMAP gr_button2;
extern BITMAP gr_button3;
extern BITMAP gr_button4;
extern BITMAP gr_button5;
extern BITMAP gr_button6;
extern BITMAP gr_button7;

BITMAP *gr_curkbd = NULL;
const int *maptable = NULL;
const int *keylen = NULL;
int kbd_numkeys = 0;

typedef struct {
  int active;
  int hold_button;
  float pointer_x, pointer_y;
  int kbd_x, kbd_y;
  struct {
    int active;
    int x_offset;
    int y_offset;
  } drag;
} KBDDATA;

static KBDDATA kbddata;

static int vkeyboard_calc_button() {
  int ymin=0, ymax=0, keylen=0, xpos=0, keycode;
  int xoff = (int)kbddata.pointer_x - kbddata.kbd_x;
  int yoff = (int)kbddata.pointer_y - kbddata.kbd_y;
  int p;

#if 0
  printf ("%d %d\n", xoff, yoff);
#endif

  if (xoff < 0 || yoff < 0)
    return -1;

  for (p=0;maptable[p]!=C_END;p++) {
    switch (maptable[p]) {
      case C_YMINMAX:
        ymin = maptable[++p];
        ymax = maptable[++p];
        break;
      case C_XPOS:
        xpos = maptable[++p];
        break;
      case C_KEYLEN:
        keylen = maptable[++p];
        break;
      case C_KEY:
        keycode = maptable[++p];
        if (yoff >= ymin && yoff <= ymax && xoff >= xpos && xoff < xpos+keylen) {
          return keycode;
        }
        xpos += keylen + 3;
        break;
    }
  }
  return -1;
}

static int vkeyboard_get_xy(int keycode, int *x, int *y) {
  int ymin=0, ymax=0, keylen=0, xpos=0;
  int p;

  for (p=0;maptable[p]!=C_END;p++) {
    switch (maptable[p]) {
      case C_YMINMAX:
        ymin = maptable[++p];
        ymax = maptable[++p];
        break;
      case C_XPOS:
        xpos = maptable[++p];
        break;
      case C_KEYLEN:
        keylen = maptable[++p];
        break;
      case C_KEY:
        if (maptable[++p] == keycode) {
          *x = xpos; *y = ymin;
          return 0;
        }        
        xpos += keylen + 3;
        break;
    }
  }

  return -1;
}

void vkeyboard_rel_button() {
  if (kbddata.hold_button == -1)
    return;
  if (pressed_keys[kbddata.hold_button])
    kbd_rel_button(kbddata.hold_button);
  pressed_keys[kbddata.hold_button] = 0;
  kbddata.hold_button = -1;
}

void vkeyboard_rel_buttons() {
  int i;
  kbddata.hold_button = -1;
  for (i=0;i<kbd_numkeys;i++) {
    if (pressed_keys[i])
      kbd_rel_button(i);
    pressed_keys[i]=0;
  }
}

void vkeyboard_reset() {
  vkeyboard_rel_buttons();
}

void vkeyboard_init() {
  bzero((char*)&kbddata, sizeof(kbddata));
  kbddata.pointer_x=100; kbddata.pointer_y=100;
  kbddata.kbd_x=82; kbddata.kbd_y=40;
  kbddata.hold_button=-1;
}

void vkeyboard_update() {
  int i;

  if (buttons.ljoy_h<90)
    kbddata.pointer_x -= (90-buttons.ljoy_h)/16.0;
  if (buttons.ljoy_h>165)
    kbddata.pointer_x += (buttons.ljoy_h-165)/16.0;

  if (buttons.ljoy_v<90)
    kbddata.pointer_y -= (90-buttons.ljoy_v)/22.0;
  if (buttons.ljoy_v>165)
    kbddata.pointer_y += (buttons.ljoy_v-165)/22.0;

#if 0
  if (new_pad & PAD_UP)
    kbddata.pointer_y--;
  if (new_pad & PAD_DOWN)
    kbddata.pointer_y++;
  if (new_pad & PAD_LEFT)
    kbddata.pointer_x--;
  if (new_pad & PAD_RIGHT)
    kbddata.pointer_x++;
#endif

  if (kbddata.pointer_y < B1_Y)
    kbddata.pointer_y = B1_Y;
  if (kbddata.pointer_y > B2_Y)
    kbddata.pointer_y = B2_Y;
  if (kbddata.pointer_x < B1_X)
    kbddata.pointer_x = B1_X;
  if (kbddata.pointer_x > B2_X)
    kbddata.pointer_x = B2_X;

  if (new_pad & PAD_CIRCLE) {
    int r;
    r = vkeyboard_calc_button();
    if (r!=-1) {
      if (!pressed_keys[r])
        kbd_press_button(r);
      pressed_keys[r] = 1;
      kbddata.hold_button = r;
    } else {
      if (kbddata.pointer_x>=kbddata.kbd_x && kbddata.pointer_y>=kbddata.kbd_y &&
          kbddata.pointer_x<kbddata.kbd_x+gr_curkbd->w &&
          kbddata.pointer_y<kbddata.kbd_y+gr_curkbd->h) {
        kbddata.drag.active = 1;
        kbddata.drag.x_offset = kbddata.kbd_x - (int)kbddata.pointer_x;
        kbddata.drag.y_offset = kbddata.kbd_y - (int)kbddata.pointer_y;
      }
    }
  } else if (new_rel & PAD_CIRCLE) {
    vkeyboard_rel_button();
    kbddata.drag.active=0;
  }

  if (new_pad & PAD_SQUARE) {
    int r;
    r = vkeyboard_calc_button();
    if (r!=-1) {
      pressed_keys[r] = !pressed_keys[r];
      if (pressed_keys[r])
        kbd_press_button(r);
      else
        kbd_rel_button(r);
    }
    if (kbddata.hold_button == r)
      kbddata.hold_button = -1;
  }

  if (kbddata.drag.active) {
    kbddata.kbd_x = (int)kbddata.pointer_x+kbddata.drag.x_offset;
    kbddata.kbd_y = (int)kbddata.pointer_y+kbddata.drag.y_offset;
    if (kbddata.kbd_x+20 >= B2_X)
      kbddata.kbd_x = B2_X-20;
    if (kbddata.kbd_y+12 >= B2_Y)
      kbddata.kbd_y = B2_Y-12;
    if (kbddata.kbd_x+gr_curkbd->w-20 < B1_X)
      kbddata.kbd_x = B1_X-gr_curkbd->w+20;
    if (kbddata.kbd_y+gr_curkbd->h-16 < B1_Y)
      kbddata.kbd_y = B1_Y-gr_curkbd->h+16;
  }

  render_bitmap (gr_curkbd, kbddata.kbd_x, kbddata.kbd_y);
  render_bitmap (&gr_pointer, kbddata.pointer_x, kbddata.pointer_y);
  for (i=0;i<kbd_numkeys;i++) {
    if (pressed_keys[i]) {
      int x, y;
      vkeyboard_get_xy(i, &x, &y);
      y-=2;
      switch (keylen[i]) {
        case L_C128ENTER:
          render_bitmap (&gr_button7, kbddata.kbd_x+x, kbddata.kbd_y+y);
          break;
        case L_NORMAL:
          render_bitmap (&gr_button1, kbddata.kbd_x+x, kbddata.kbd_y+y);
          break;
        case L_SPACE:
          render_bitmap (&gr_button4, kbddata.kbd_x+x, kbddata.kbd_y+y);
          break;
        case L_CTRL:
          render_bitmap (&gr_button2, kbddata.kbd_x+x, kbddata.kbd_y+y);
          break;
        case L_RETURN:
          render_bitmap (&gr_button3, kbddata.kbd_x+x, kbddata.kbd_y+y);
          break;
        case L_PLUS4F:
          render_bitmap (&gr_button6, kbddata.kbd_x+x, kbddata.kbd_y+y);
          break;
        case L_PLUS4CRS:
          render_bitmap (&gr_button5, kbddata.kbd_x+x, kbddata.kbd_y+y);
          break;
      }
    }
  }
}

void vkeyboard_frame() {
  if (new_pad & PAD_R1) {
    kbddata.active = !kbddata.active;
    vkeyboard_rel_buttons();
  }
  if (kbddata.active)
    vkeyboard_update();
}
