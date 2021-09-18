/*
 * plus4ui.c - C64-specific user interface.
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

extern "C" {
#include "constants.h" 
#include "plus4ui.h"
#include "resources.h"
#include "ui.h"
}

ui_menu_toggle  plus4_ui_menu_toggles[] = {
    { "TEDDoubleSize", MENU_TOGGLE_DOUBLESIZE },
    { "TEDDoubleScan", MENU_TOGGLE_DOUBLESCAN },
    { "TEDVideoCache", MENU_TOGGLE_VIDEOCACHE },
    { "TEDScale2x", MENU_TOGGLE_SCALE2X },
    { "SidCart", MENU_TOGGLE_SIDCART },
    { "SidFilters", MENU_TOGGLE_SIDCART_FILTERS },
    { NULL, 0 }
};

ui_res_possible_values plus4_SIDCARTModel[] = {
        {0, MENU_SIDCART_MODEL_6581},
        {1, MENU_SIDCART_MODEL_8580},
        {-1, 0}
};

ui_res_possible_values plus4_SIDCARTAddress[] = {
        {0, MENU_SIDCART_ADDRESS_1},
        {1, MENU_SIDCART_ADDRESS_2},
        {-1, 0}
};

ui_res_possible_values plus4_SIDCARTClock[] = {
        {0, MENU_SIDCART_CLOCK_C64},
        {1, MENU_SIDCART_CLOCK_NATIVE},
        {-1, 0}
};

ui_res_value_list plus4_ui_res_values[] = {
    {"SidModel", plus4_SIDCARTModel},
    {"SidAddress", plus4_SIDCARTAddress},
    {"SidClock", plus4_SIDCARTClock},
    { NULL, NULL }
};

static void plus4_ui_specific(void *msg, void *window)
{
}

int plus4ui_init(void)
{
    ui_register_machine_specific(plus4_ui_specific);
    ui_register_menu_toggles(plus4_ui_menu_toggles);
    ui_register_res_values(plus4_ui_res_values);
    return 0;
}

void plus4ui_shutdown(void)
{
}

