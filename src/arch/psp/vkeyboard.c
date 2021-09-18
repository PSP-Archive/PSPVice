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
#include "types.h"

#include <string.h>
#include "pad.h"
#include "vkeyboard.h"
#include "bitmap.h"
#include "via.h"
#include "kbd.h"
#include "PSPInputs.h"

extern int disp_w;
extern int disp_h;

#define B1_X 10
#define B2_X (disp_w-10)
#define B1_Y 5
#define B2_Y (disp_h-5)

// -- 256 keys
unsigned char pressed_keys[256] = {0, 0}; // State of keys (0=not pressed 1=pressed)

static int vkeyboardinitdone=0;

//unsigned char joystick_buttons[5] = {0};

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
extern BITMAP gr_button_space;
extern BITMAP gr_button_return;
extern BITMAP gr_button_ctrl;
extern BITMAP gr_button_fkey;
extern BITMAP gr_button_normal;
extern BITMAP gr_keyboard;
extern BITMAP gr_keyboard_shifted;

// Keyboard Settings (specific to machine)
BITMAP* gr_curkbd = &gr_keyboard; // Can also be &gr_keyboard_shifted
const int *maptable = NULL;
const int *keylen = NULL;
int kbd_numkeys = 0; // Number of keys on keyboard. C64 have 66 keys

typedef struct
{
  int   active;               // Do keyboard is displayed on screen ? (bool)
  int   hold_button;          // Id of button which is currently hold
  float pointer_x, pointer_y; // Pointer coordinates
  int   kbd_x, kbd_y;         // Position of keyboard on screen
  struct
  {
    int active;
    int x_offset;
    int y_offset;
  } drag;
} KBDDATA;

static KBDDATA kbddata;

#define SPACE_BETWEEN_KEYS 0 

// ------------------------------------
// Calc the key under pointer on screen
// return id (0 to 255)
// ------------------------------------
static int vkeyboard_calc_button()
{
  int ymin=0, ymax=0, keylen=0, xpos=0, keycode;
  int xoff = (int)kbddata.pointer_x - kbddata.kbd_x;
  int yoff = (int)kbddata.pointer_y - kbddata.kbd_y;
  int p;

#if 0
  printf ("%d %d\n", xoff, yoff);
#endif

  if (xoff < 0 || yoff < 0)
    return -1;

  for (p=0;maptable[p]!=C_END;p++)
  {
    switch (maptable[p])
    {
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
        if ( (yoff >= ymin) && (yoff <= ymax) && (xoff >= xpos) && (xoff < xpos+keylen) )
        {
          return keycode;
        }
        xpos += keylen + SPACE_BETWEEN_KEYS;
        break;
    }
  }
  return -1;
}

static int vkeyboard_get_xy(int keycode, int *x, int *y)
{
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
        xpos += keylen + SPACE_BETWEEN_KEYS;
        break;
    }
  }

  return -1;
}

void vkeyboard_rel_button()
{
  if (kbddata.hold_button == -1)
    return;
  if (pressed_keys[kbddata.hold_button])
    kbd_rel_button(kbddata.hold_button); // Press keys in Vice
  pressed_keys[kbddata.hold_button] = 0;
  kbddata.hold_button = -1;
}

void vkeyboard_rel_buttons()
{
  int i;
  kbddata.hold_button = -1;
  for (i=0;i<kbd_numkeys;i++)
  {
    if (pressed_keys[i])
      kbd_rel_button(i); // Press keys in Vice
    pressed_keys[i]=0;
  }
}

void vkeyboard_reset()
{
  vkeyboard_rel_buttons();
}

void vkeyboard_init()
{
  bzero((char*)&kbddata, sizeof(kbddata));
  kbddata.active=1;
  
  kbddata.pointer_x=100; // Init position of pointer
  kbddata.pointer_y=100;
  kbddata.kbd_x=(480-298)/2; // Init position of keyboard (center)
  kbddata.kbd_y=(272-104)/2;
  kbddata.hold_button=-1;
}

void vkeyboard_update()
{
  int i;

  if (vkeyboardinitdone==0)
  {
    vkeyboard_init();
    vkeyboardinitdone=1;
  }

  // 0=joystick pushed to the left .... 255 joystick pushed to the right .... 90 to 165, dead zone
  
  #define KEYB_MIN (128.f-30.0f)
  #define KEYB_MAX (128.f+30.0f)
  #define KEYB_STEPX 50.f
  #define KEYB_STEPY 80.f
  
  if (buttons.ljoy_h<KEYB_MIN)
    kbddata.pointer_x -= (KEYB_MIN-buttons.ljoy_h)/KEYB_STEPX;
  if (buttons.ljoy_h>KEYB_MAX)
    kbddata.pointer_x += (buttons.ljoy_h-KEYB_MAX)/KEYB_STEPX;

  if (buttons.ljoy_v<KEYB_MIN)
    kbddata.pointer_y -= (KEYB_MIN-buttons.ljoy_v)/KEYB_STEPY;
  if (buttons.ljoy_v>KEYB_MAX)
    kbddata.pointer_y += (buttons.ljoy_v-KEYB_MAX)/KEYB_STEPY;

  if (kbddata.pointer_y < B1_Y)
    kbddata.pointer_y = B1_Y;
  if (kbddata.pointer_y > B2_Y)
    kbddata.pointer_y = B2_Y;
  if (kbddata.pointer_x < B1_X)
    kbddata.pointer_x = B1_X;
  if (kbddata.pointer_x > B2_X)
    kbddata.pointer_x = B2_X;

  // -- If X button is pushed, a key is pressed
  if (new_pad & PAD_CROSS)
  {
    int r;
    r = vkeyboard_calc_button(); // r = Id of key pressed
    if (r!=-1)
    {
      // Key "r" is pressed
      if (!pressed_keys[r])
        kbd_press_button(r); // Press keys in Vice
      pressed_keys[r] = 1;
      kbddata.hold_button = r;
    }
    else
    {
      // -- No key is pressed, so drag keyboard
      if (kbddata.pointer_x>=kbddata.kbd_x && kbddata.pointer_y>=kbddata.kbd_y &&
          kbddata.pointer_x<kbddata.kbd_x+gr_curkbd->w &&
          kbddata.pointer_y<kbddata.kbd_y+gr_curkbd->h)
      {
        kbddata.drag.active = 1;
        kbddata.drag.x_offset = kbddata.kbd_x - (int)kbddata.pointer_x;
        kbddata.drag.y_offset = kbddata.kbd_y - (int)kbddata.pointer_y;
      }
    }
  }
  else if (new_rel & PAD_CROSS) // Button is release
  {
    vkeyboard_rel_button();
    kbddata.drag.active=0;
  }
  
  
  // -- Square = Hold Shift (code 39)
  if (new_pad & PAD_SQUARE)
  {
    pressed_keys[K_LSHIFT]=1;
    kbd_press_button(K_LSHIFT); // Press keys in Vice
  }
  if ( new_rel & PAD_SQUARE )
  {
    pressed_keys[K_LSHIFT]=0;
    kbd_rel_button(K_LSHIFT); // Press keys in Vice
  }


  // -- Circle = Hold CTRL (code 16)
  if (new_pad & PAD_CIRCLE)
  {
    pressed_keys[K_CTRL]=1;
    kbd_press_button(K_CTRL); // Press keys in Vice
  }
  if ( new_rel & PAD_CIRCLE )
  {
    pressed_keys[K_CTRL]=0;
    kbd_rel_button(K_CTRL); // Press keys in Vice
  }
  
  // -- Change Keyboard GFX if shift is press only
  if ( pressed_keys[K_LSHIFT] && ! (pressed_keys[K_CTRL]) )
  {
    gr_curkbd = &gr_keyboard_shifted;
  
  }
  else
  {
    gr_curkbd = &gr_keyboard;
  }
  
  if (kbddata.drag.active)
  {
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

  for (i=0;i<kbd_numkeys;i++)
  {
    if (pressed_keys[i]) {
      int x, y;
      vkeyboard_get_xy(i, &x, &y);
      y+=1; // CK Center bitmap
      switch (keylen[i])
      {
        case L_NORMAL:
          render_bitmap (&gr_button_normal, kbddata.kbd_x+x, kbddata.kbd_y+y);
          break;
        case L_SPACE:
          render_bitmap (&gr_button_space, kbddata.kbd_x+x, kbddata.kbd_y+y);
          break;
        case L_CTRL:
          render_bitmap (&gr_button_ctrl, kbddata.kbd_x+x, kbddata.kbd_y+y);
          break;
        case L_FKEY:
          render_bitmap (&gr_button_fkey, kbddata.kbd_x+x, kbddata.kbd_y+y);
          break;
        case L_RETURN:
          render_bitmap (&gr_button_return, kbddata.kbd_x+x, kbddata.kbd_y+y);
          break;
      }
    }
  }
  
}

void vkeyboard_frame()
{
  if (PSPInputs_IsButtonJustPressed(pspinp_DisplayKeyboard))
  {
    kbddata.active = !kbddata.active;
    vkeyboard_rel_buttons();
  }
  if (kbddata.active)
    vkeyboard_update();
}

void vkeyboard_activate()
{
  kbddata.active = 1;
}

int vkeyboard_IsActive()
{
	return kbddata.active;
}