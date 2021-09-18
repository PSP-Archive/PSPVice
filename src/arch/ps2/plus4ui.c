/*
 * plus4ui.c - Definition of the PLUS4-specific part of the UI.
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
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

#include <stdio.h>
#include <stdlib.h>

#include "lib.h"
#include "menudefs.h"
#include "plus4ui.h"
#include "resources.h"
#include "ui.h"
#include "uisid.h"
#include "menu.h"
#include "vkeyboard.h"

int plus4ui_init(void)
{
    set_machine_menu(MENU_PLUS4);
    vkeyboard_set_plus4();
    return 0;
}

void plus4ui_shutdown(void)
{
}

