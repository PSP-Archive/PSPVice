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
#include <malloc.h>
#include <tamtypes.h>
#include "libkbd.h"
#include "kbd.h"
#include "via.h"
#include "vkeyboard.h"

static PS2KbdRawKey *key;

static const int foo[129] = {0};

const unsigned char *usbkeymap = NULL;
const unsigned char *usbkeyval = NULL;

int usbkeymaplen = 0;

void ps2_keyboard_init() {
  key = memalign(64, sizeof(PS2KbdRawKey));
  PS2KbdInit();
  PS2KbdSetReadmode(PS2KBD_READMODE_RAW);
  PS2KbdSetBlockingMode(PS2KBD_NONBLOCKING);
}

void keyboard_read() {
  int i;
  while (PS2KbdReadRaw(key)) {
    for (i=0;i<usbkeymaplen;i++) {
      if (usbkeymap[i] == key->key) {
        if (key->state == PS2KBD_RAWKEY_DOWN) {
          if (!pressed_keys[usbkeyval[i]])
            kbd_press_button(usbkeyval[i]);
          pressed_keys[usbkeyval[i]] = 1;
        } else {
          if (pressed_keys[usbkeyval[i]])
            kbd_rel_button(usbkeyval[i]);
          pressed_keys[usbkeyval[i]] = 0;
        }
      }
    }
//    printf ("key = %d\n", key->key);
  }
}

const char foo_[4];
