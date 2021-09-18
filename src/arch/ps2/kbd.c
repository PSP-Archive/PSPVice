/*
 * kbd.c - MS-DOS keyboard driver.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <viceteam@t-online.de>
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

#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ps2kbd.h"
#include "cmdline.h"
#include "datasette.h"
#include "fliplist.h"
#include "joy.h"
#include "kbd.h"
#include "keyboard.h"
#include "log.h"
#include "machine.h"
#include "mem.h"
#include "resources.h"
#include "types.h"
#include "ui.h"
#include "vsync.h"

/* Function for triggering cartridge (e.g. AR) freezing.  */
static void (*freeze_function)(void);

void kbd_set_freeze_function(void (*f)())
{
    freeze_function = f;
}

void kbd_arch_init(void)
{
}

signed long kbd_arch_keyname_to_keynum(char *keyname)
{
    return (signed long)atoi(keyname);
}

const char *kbd_arch_keynum_to_keyname(signed long keynum)
{
    static char keyname[20];

    memset(keyname, 0, 20);

    sprintf(keyname, "%li", keynum);

    return keyname;
}

void kbd_update(void) {
  keyboard_read();
}

void kbd_press_button(int b) {
  keyboard_key_pressed(b+1);
}

void kbd_rel_button(int b) {
  keyboard_key_released(b+1);
}
