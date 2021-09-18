/*
 * kbd.h - PS2 keyboard driver.
 *
 * Written by
 *  Ettore Perazzoli <raipsu@users.sourceforge.net>
 * Based on the X11 code by
 *  Jouko Valta <jopi@stekt.oulu.fi>
 *  Andre' Fachat <fachat@physik.tu-chemnitz.de>
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

#ifndef _KBD_PS2_H
#define _KBD_PS2_H

#include "types.h"

extern void kbd_arch_init(void);
extern void kbd_update(void);

extern signed long kbd_arch_keyname_to_keynum(char *keyname);
extern const char *kbd_arch_keynum_to_keyname(signed long keynum);

#define KBD_C64_SYM_US  "psp_sym_us.vkm"
#define KBD_C64_SYM_DE  "psp_sym_de.vkm"
#define KBD_C64_POS     "psp_pos.vkm"
#define KBD_C128_SYM    "psp_sym.vkm"
#define KBD_C128_POS    "psp_pos.vkm"
#define KBD_VIC20_SYM   "psp_sym.vkm"
#define KBD_VIC20_POS   "psp_pos.vkm"
#define KBD_PET_SYM_UK  "psp_sym_uk.vkm"
#define KBD_PET_POS_UK  "psp_pos_uk.vkm"
#define KBD_PET_SYM_DE  "psp_sym_de.vkm"
#define KBD_PET_POS_DE  "psp_pos_de.vkm"
#define KBD_PET_SYM_GR  "psp_sym_gr.vkm"
#define KBD_PET_POS_GR  "psp_pos_gr.vkm"
#define KBD_PLUS4_SYM   "psp_sym.vkm"
#define KBD_PLUS4_POS   "psp_pos.vkm"
#define KBD_CBM2_SYM_UK "psp_sym_uk.vkm"
#define KBD_CBM2_POS_UK "psp_pos_uk.vkm"
#define KBD_CBM2_SYM_DE "psp_sym_de.vkm"
#define KBD_CBM2_POS_DE "psp_pos_de.vkm"
#define KBD_CBM2_SYM_GR "psp_sym_gr.vkm"
#define KBD_CBM2_POS_GR "psp_pos_gr.vkm" 


#define KBD_INDEX_C64_DEFAULT   KBD_INDEX_C64_POS
#define KBD_INDEX_C128_DEFAULT  KBD_INDEX_C128_POS
#define KBD_INDEX_VIC20_DEFAULT KBD_INDEX_VIC20_POS
#define KBD_INDEX_PET_DEFAULT   KBD_INDEX_PET_BUKP
#define KBD_INDEX_PLUS4_DEFAULT KBD_INDEX_PLUS4_POS
#define KBD_INDEX_CBM2_DEFAULT  KBD_INDEX_CBM2_BUKP

void kbd_press_button(int b);
void kbd_rel_button(int b);

#endif

