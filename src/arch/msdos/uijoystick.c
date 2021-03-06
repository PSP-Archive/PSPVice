/*
 * uijoystick.c
 *
 * Written by
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

#include <conio.h>
#include <stdio.h>

#include <allegro.h>

#include "joy.h"
#include "lib.h"
#include "grabkey.h"
#include "resources.h"
#include "tui.h"
#include "tui_backend.h"
#include "tuimenu.h"
#include "ui.h"
#include "uijoystick.h"


static TUI_MENU_CALLBACK(get_joystick_device_callback)
{
    int port = (int) param;
    char *resource = port == 1 ? "JoyDevice1" : "JoyDevice2";
    int value;

    resources_get_int(resource, &value);
    switch (value) {
      case JOYDEV_NONE:
        return "None";
      case JOYDEV_NUMPAD:
        return "Numpad + Right Ctrl";
      case JOYDEV_KEYSET1:
        return "Keyset A";
      case JOYDEV_KEYSET2:
        return "Keyset B";
      case JOYDEV_HW1:
        return "Joystick #1";
      case JOYDEV_HW2:
        return "Joystick #2";
    }

    return "Unknown";
}

static TUI_MENU_CALLBACK(get_hw_joystick_type_callback)
{
    int value;

    resources_get_int("HwJoyType", &value);
    switch (value) {
      case 0:
        return "None";
#ifdef JOY_TYPE_STANDARD
      case JOY_TYPE_STANDARD:
        return "Standard";
#endif
#ifdef JOY_TYPE_2PADS
      case JOY_TYPE_2PADS:
        return "2 pads";
#endif
#ifdef JOY_TYPE_4BUTTON
      case JOY_TYPE_4BUTTON:
        return "4 buttons";
#endif
#ifdef JOY_TYPE_6BUTTON
      case JOY_TYPE_6BUTTON:
        return "6 buttons";
#endif
#ifdef JOY_TYPE_8BUTTON
      case JOY_TYPE_8BUTTON:
        return "8 buttons";
#endif
#ifdef JOY_TYPE_FSPRO
      case JOY_TYPE_FSPRO:
        return "Flightstick Pro";
#endif
#ifdef JOY_TYPE_WINGEX
      case JOY_TYPE_WINGEX:
        return "Wingman Extreme";
#endif
#ifdef JOY_TYPE_SIDEWINDER
      case JOY_TYPE_SIDEWINDER:
        return "Sidewinder";
#endif
#ifdef JOY_TYPE_SIDEWINDER_AG
      case JOY_TYPE_SIDEWINDER_AG:
        return "Sidewinder Aggressive";
#endif
#ifdef JOY_TYPE_GAMEPAD_PRO
      case JOY_TYPE_GAMEPAD_PRO:
        return "GamePad Pro";
#endif
#ifdef JOY_TYPE_GRIP
      case JOY_TYPE_GRIP:
        return "GrIP";
#endif
#ifdef JOY_TYPE_GRIP4
      case JOY_TYPE_GRIP4:
        return "GrIP 4-way";
#endif
#ifdef JOY_TYPE_SNESPAD_LPT1
      case JOY_TYPE_SNESPAD_LPT1:
        return "SNESpad on LPT1";
#endif
#ifdef JOY_TYPE_SNESPAD_LPT2
      case JOY_TYPE_SNESPAD_LPT2:
        return "SNESpad on LPT2";
#endif
#ifdef JOY_TYPE_SNESPAD_LPT3
      case JOY_TYPE_SNESPAD_LPT3:
        return "SNESpad on LPT3";
#endif
#ifdef JOY_TYPE_PSXPAD_LPT1
      case JOY_TYPE_PSXPAD_LPT1:
        return "PSXpad on LPT1";
#endif
#ifdef JOY_TYPE_PSXPAD_LPT2
      case JOY_TYPE_PSXPAD_LPT2:
        return "PSXpad on LPT2";
#endif
#ifdef JOY_TYPE_PSXPAD_LPT3
      case JOY_TYPE_PSXPAD_LPT3:
        return "PSXpad on LPT3";
#endif
#ifdef JOY_TYPE_N64PAD_LPT1
      case JOY_TYPE_N64PAD_LPT1:
        return "N64pad on LPT1";
#endif
#ifdef JOY_TYPE_N64PAD_LPT2
      case JOY_TYPE_N64PAD_LPT2:
        return "N64pad on LPT2";
#endif
#ifdef JOY_TYPE_N64PAD_LPT3
      case JOY_TYPE_N64PAD_LPT3:
        return "N64pad on LPT3";
#endif
#ifdef JOY_TYPE_DB9_LPT1
      case JOY_TYPE_DB9_LPT1:
        return "DB9 on LPT1";
#endif
#ifdef JOY_TYPE_DB9_LPT2
      case JOY_TYPE_DB9_LPT2:
        return "DB9 on LPT2";
#endif
#ifdef JOY_TYPE_DB9_LPT3
      case JOY_TYPE_DB9_LPT3:
        return "DB9 on LPT3";
#endif
#ifdef JOY_TYPE_TURBOGRAFX_LPT1
      case JOY_TYPE_TURBOGRAFX_LPT1:
        return "TurbograFX on LPT1";
#endif
#ifdef JOY_TYPE_TURBOGRAFX_LPT2
      case JOY_TYPE_TURBOGRAFX_LPT2:
        return "TurbograFX on LPT2";
#endif
#ifdef JOY_TYPE_TURBOGRAFX_LPT3
      case JOY_TYPE_TURBOGRAFX_LPT3:
        return "TurbograFX on LPT3";
#endif
#ifdef JOY_TYPE_IFSEGA_ISA
      case JOY_TYPE_IFSEGA_ISA:
        return "IF-SEGA/ISA";
#endif
#ifdef JOY_TYPE_IFSEGA_PCI
      case JOY_TYPE_IFSEGA_PCI:
        return "IF-SEGA2/PCI";
#endif
#ifdef JOY_TYPE_IFSEGA_PCI_FAST
      case JOY_TYPE_IFSEGA_PCI_FAST:
        return "IF-SEGA2/PCI (normal)";
#endif
#ifdef JOY_TYPE_WINGWARRIOR
      case JOY_TYPE_WINGWARRIOR:
        return "Wingman Warrior";
#endif
    }

    return "Unknown";
}

static TUI_MENU_CALLBACK(set_joy_device_callback)
{
    int port = (int)param >> 8;
    char *resource = port == 1 ? "JoyDevice1" : "JoyDevice2";

    if (been_activated) {
        resources_set_int(resource, ((int)param & 0xff));
        ui_update_menus();
    } else {
        int value;

        resources_get_int(resource, &value);
        if (value == ((int)param & 0xff))
            *become_default = 1;
    }

    return NULL;
}

static TUI_MENU_CALLBACK(joy_hw_callback)
{
    if (been_activated) {
        resources_set_int("HwJoyType", (int)(param));
        ui_update_menus();
    } else {
        int value;

        resources_get_int("HwJoyType", &value);
        if (value == ((int)param))
            *become_default = 1;
    }

    return NULL;
}

static TUI_MENU_CALLBACK(swap_joysticks_callback)
{
    int value1, value2, tmp;

    if (been_activated) {
        resources_get_int("JoyDevice1", &value1);
        resources_get_int("JoyDevice2", &value2);

        tmp = value1;
        value1 = value2;
        value2 = tmp;

        resources_set_int("JoyDevice1", value1);
        resources_set_int("JoyDevice2", value2);

        ui_update_menus();
    }

    return NULL;
}

static tui_menu_item_def_t joy_device_1_submenu[] = {
    { "N_one",
      "No joystick device attached",
      set_joy_device_callback, (void *)(0x100 | JOYDEV_NONE), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "--" },
    { "_Numpad + Right Ctrl",
      "Use numeric keypad for movement and right Ctrl for fire",
      set_joy_device_callback, (void *)(0x100 | JOYDEV_NUMPAD), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Keyset _A",
      "Use keyset A",
      set_joy_device_callback, (void *)(0x100 | JOYDEV_KEYSET1), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Keyset _B",
      "Use keyset B",
      set_joy_device_callback, (void *)(0x100 | JOYDEV_KEYSET2), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "--" },
    { "PC Joystick #_1",
      "Use real PC joystick #1",
      set_joy_device_callback, (void *)(0x100 | JOYDEV_HW1), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "PC Joystick #_2",
      "Use real PC joystick #2",
      set_joy_device_callback, (void *)(0x100 | JOYDEV_HW2), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static tui_menu_item_def_t joy_device_2_submenu[] = {
    { "N_one",
      "No joystick device attached",
      set_joy_device_callback, (void *)(0x200 | JOYDEV_NONE), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "--" },
    { "_Numpad + Right Ctrl",
      "Use numeric keypad for movement and right Ctrl for fire",
      set_joy_device_callback, (void *)(0x200 | JOYDEV_NUMPAD), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Keyset _A",
      "Use keyset A",
      set_joy_device_callback, (void *)(0x200 | JOYDEV_KEYSET1), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Keyset _B",
      "Use keyset B",
      set_joy_device_callback, (void *)(0x200 | JOYDEV_KEYSET2), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "--" },
    { "Joystick #_1",
      "Use real joystick #1",
      set_joy_device_callback, (void *)(0x200 | JOYDEV_HW1), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Joystick #_2",
      "Use real joystick #2",
      set_joy_device_callback, (void *)(0x200 | JOYDEV_HW2), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

typedef enum {
    KEYSET_NW,
    KEYSET_N,
    KEYSET_NE,
    KEYSET_E,
    KEYSET_SE,
    KEYSET_S,
    KEYSET_SW,
    KEYSET_W,
    KEYSET_FIRE
} joystick_direction_t;

static const char *joystick_direction_to_string(joystick_direction_t
                                                direction)
{
    static char *s[] = {
        "NorthWest", "North", "NorthEast", "East",
        "SouthEast", "South", "SouthWest", "West",
        "Fire"
    };

    return s[(int)direction];
}

static TUI_MENU_CALLBACK(keyset_callback)
{
    int direction, number;
    int value;
    char *rname;

    number = (int)param >> 8;
    direction = (int)param & 0xff;
    rname = lib_msprintf("KeySet%d%s", number,
                         joystick_direction_to_string(direction));

    if (been_activated) {
        kbd_code_t key;
        int width = 60, height = 5;
        int x = CENTER_X(width), y = CENTER_Y(height);
        tui_area_t backing_store = NULL;
        char *msg;

        msg = lib_msprintf("Press key for %s%s (Esc for none)...",
                           direction == KEYSET_FIRE ? "" : "direction ",
                           joystick_direction_to_string(direction));
        tui_display_window(x, y, width, height, MESSAGE_BORDER, MESSAGE_BACK,
                           NULL, &backing_store);
        tui_set_attr(MESSAGE_FORE, MESSAGE_BACK, 0);
        tui_display(CENTER_X(strlen(msg)), y + 2, 0, msg);
        lib_free(msg);

        /* Do not allow Alt as we need it for hotkeys.  */
        do
            key = grab_key();
        while (key == K_LEFTALT || key == K_RIGHTALT);

        tui_area_put(backing_store, x, y);
        tui_area_free(backing_store);

        if (key == K_ESC)
            key = K_NONE;
        resources_set_int(rname, (int)key);
    }

    resources_get_int(rname, &value);
    lib_free(rname);
    return kbd_code_to_string((kbd_code_t)value);
}

#define DEFINE_KEYSET_MENU(num)                                         \
    static tui_menu_item_def_t keyset_##num##_submenu[] = {             \
        { "_1: South West:",                                            \
          "Specify key for diagonal down-left direction",               \
          keyset_callback, (void *)((num << 8) | KEYSET_SW), 12,        \
          TUI_MENU_BEH_CONTINUE, NULL, NULL },                          \
        { "_2: South:",                                                 \
          "Specify key for the down direction",                         \
          keyset_callback, (void *)((num << 8) | KEYSET_S), 12,         \
          TUI_MENU_BEH_CONTINUE, NULL, NULL },                          \
        { "_3: South East:",                                            \
          "Specify key for the diagonal down-right direction",          \
          keyset_callback, (void *)((num << 8) | KEYSET_SE), 12,        \
          TUI_MENU_BEH_CONTINUE, NULL, NULL },                          \
        { "_4: West:",                                                  \
          "Specify key for the left direction",                         \
          keyset_callback, (void *)((num << 8) | KEYSET_W), 12,         \
          TUI_MENU_BEH_CONTINUE, NULL, NULL },                          \
        { "_6: East:",                                                  \
          "Specify key for the right direction",                        \
          keyset_callback, (void *)((num << 8) | KEYSET_E), 12,         \
          TUI_MENU_BEH_CONTINUE, NULL, NULL },                          \
        { "_7: North West:",                                            \
          "Specify key for the diagonal up-left direction",             \
          keyset_callback, (void *)((num << 8) | KEYSET_NW), 12,        \
          TUI_MENU_BEH_CONTINUE, NULL, NULL },                          \
        { "_8: North:",                                                 \
          "Specify key for the up direction",                           \
          keyset_callback, (void *)((num << 8) | KEYSET_N), 12,         \
          TUI_MENU_BEH_CONTINUE, NULL, NULL },                          \
        { "_9: North East:",                                            \
          "Specify key for the diagonal up-right direction",            \
          keyset_callback, (void *)((num << 8) | KEYSET_NE), 12,        \
          TUI_MENU_BEH_CONTINUE, NULL, NULL },                          \
        { "_0: Fire",                                                   \
          "Specify key for the fire button",                            \
          keyset_callback, (void *)((num << 8) | KEYSET_FIRE), 12,      \
          TUI_MENU_BEH_CONTINUE, NULL, NULL },                          \
        { NULL }                                                        \
    };

DEFINE_KEYSET_MENU(1)
DEFINE_KEYSET_MENU(2)

static tui_menu_item_def_t joy_list_submenu[] = {
        { "N_one",
          "No PC joystick",
          joy_hw_callback, (void *)0, 0,
          TUI_MENU_BEH_CLOSE, NULL, NULL },
#ifdef JOY_TYPE_STANDARD
        { "_Standard",
          "Standard PC joystick",
          joy_hw_callback, (void *)JOY_TYPE_STANDARD, 0,
          TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef JOY_TYPE_2PADS
        { "_2 pads",
          "Standard PC joystick (2 pads)",
          joy_hw_callback, (void *)JOY_TYPE_2PADS, 0,
          TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef JOY_TYPE_4BUTTON
        { "_4 buttons",
          "Standard PC joystick (4 buttons)",
          joy_hw_callback, (void *)JOY_TYPE_4BUTTON, 0,
          TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef JOY_TYPE_6BUTTON
        { "_6 buttons",
          "Standard PC joystick (6 buttons)",
          joy_hw_callback, (void *)JOY_TYPE_8BUTTON, 0,
          TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef JOY_TYPE_8BUTTON
        { "_8 buttons",
          "Standard PC joystick (8 buttons)",
          joy_hw_callback, (void *)JOY_TYPE_8BUTTON, 0,
          TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef JOY_TYPE_FSPRO
        { "_Flightstick Pro",
          "Flightstick Pro",
          joy_hw_callback, (void *)JOY_TYPE_FSPRO, 0,
          TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef JOY_TYPE_WINGEX
        { "_Wingman Extreme",
          "Wingman Extreme",
          joy_hw_callback, (void *)JOY_TYPE_WINGEX, 0,
          TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef JOY_TYPE_SIDEWINDER
        { "S_idewinder",
          "Sidewinder",
          joy_hw_callback, (void *)JOY_TYPE_SIDEWINDER, 0,
          TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef JOY_TYPE_SIDEWINDER_AG
        { "Sidewinder Aggressi_ve",
          "Sidewinder Aggressive",
          joy_hw_callback, (void *)JOY_TYPE_SIDEWINDER_AG, 0,
          TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef JOY_TYPE_GAMEPAD_PRO
        { "_GamePad Pro",
          "GamePad Pro",
          joy_hw_callback, (void *)JOY_TYPE_GAMEPAD_PRO, 0,
          TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef JOY_TYPE_GRIP
        { "G_rIP",
          "Gravis GrIP",
          joy_hw_callback, (void *)JOY_TYPE_GRIP, 0,
          TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef JOY_TYPE_GRIP4
        { "GrI_P 4-way",
          "Gravis GrIP (4-way)",
          joy_hw_callback, (void *)JOY_TYPE_GRIP4, 0,
          TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef JOY_TYPE_SNESPAD_LPT1
        { "SN_ESpad on LPT1",
          "Super Nintendo pad connected to parallel port 1",
          joy_hw_callback, (void *)JOY_TYPE_SNESPAD_LPT1, 0,
          TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef JOY_TYPE_SNESPAD_LPT2
        { "SNESpad on LPT2",
          "Super Nintendo pad connected to parallel port 2",
          joy_hw_callback, (void *)JOY_TYPE_SNESPAD_LPT2, 0,
          TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef JOY_TYPE_SNESPAD_LPT3
        { "SNESpad on LPT3",
          "Super Nintendo pad connected to parallel port 3",
          joy_hw_callback, (void *)JOY_TYPE_SNESPAD_LPT3, 0,
          TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef JOY_TYPE_PSXPAD_LPT1
        { "PS_Xpad on LPT1",
          "Playstation pad connected to parallel port 1",
          joy_hw_callback, (void *)JOY_TYPE_PSXPAD_LPT1, 0,
          TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef JOY_TYPE_PSXPAD_LPT2
        { "PSXpad on LPT2",
          "Playstation pad connected to parallel port 2",
          joy_hw_callback, (void *)JOY_TYPE_PSXPAD_LPT2, 0,
          TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef JOY_TYPE_PSXPAD_LPT3
        { "PSXpad on LPT3",
          "Playstation pad connected to parallel port 3",
          joy_hw_callback, (void *)JOY_TYPE_PSXPAD_LPT3, 0,
          TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef JOY_TYPE_N64PAD_LPT1
        { "_N64pad on LPT1",
          "Nintendo 64 pad connected to parallel port 1",
          joy_hw_callback, (void *)JOY_TYPE_N64PAD_LPT1, 0,
          TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef JOY_TYPE_N64PAD_LPT2
        { "N64pad on LPT2",
          "Nintendo 64 pad connected to parallel port 2",
          joy_hw_callback, (void *)JOY_TYPE_N64PAD_LPT2, 0,
          TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef JOY_TYPE_N64PAD_LPT3
        { "N64pad on LPT3",
          "Nintendo 64 pad connected to parallel port 3",
          joy_hw_callback, (void *)JOY_TYPE_N64PAD_LPT3, 0,
          TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef JOY_TYPE_DB9_LPT1
        { "DB_9 on LPT1",
          "Commodore 64 joystick connected to parallel port 1 with DB9 interface",
          joy_hw_callback, (void *)JOY_TYPE_DB9_LPT1, 0,
          TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef JOY_TYPE_DB9_LPT2
        { "DB9 on LPT2",
          "Commodore 64 joystick connected to parallel port 2 with DB9 interface",
          joy_hw_callback, (void *)JOY_TYPE_DB9_LPT2, 0,
          TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef JOY_TYPE_DB9_LPT3
        { "DB9 on LPT3",
          "Commodore 64 joystick connected to parallel port 3 with DB9 interface",
          joy_hw_callback, (void *)JOY_TYPE_DB9_LPT3, 0,
          TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef JOY_TYPE_TURBOGRAFX_LPT1
        { "_TurbograFX on LPT1",
          "Commodore 64 joystick connected to parallel port 1 with TurbograFX interface",
          joy_hw_callback, (void *)JOY_TYPE_TURBOGRAFX_LPT1, 0,
          TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef JOY_TYPE_TURBOGRAFX_LPT2
        { "TurbograFX on LPT2",
          "Commodore 64 joystick connected to parallel port 2 with TurbograFX interface",
          joy_hw_callback, (void *)JOY_TYPE_TURBOGRAFX_LPT2, 0,
          TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef JOY_TYPE_TURBOGRAFX_LPT3
        { "TurbograFX on LPT3",
          "Commodore 64 joystick connected to parallel port 3 with TurbograFX interface",
          joy_hw_callback, (void *)JOY_TYPE_TURBOGRAFX_LPT3
, 0,
          TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef JOY_TYPE_IFSEGA_ISA
        { "IF-SEGA/IS_A",
          "IF-SEGA (ISA)",
          joy_hw_callback, (void *)JOY_TYPE_IFSEGA_ISA, 0,
          TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef JOY_TYPE_IFSEGA_PCI
        { "IF-SEGA2/P_CI",
          "IF-SEGA2/PCI",
          joy_hw_callback, (void *)JOY_TYPE_IFSEGA_PCI, 0,
          TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef JOY_TYPE_IFSEGA_PCI_FAST
        { "IF-SEGA2/PCI (norma_l)",
          "IF-SEGA2/PCI (normal)",
          joy_hw_callback, (void *)JOY_TYPE_IFSEGA_PCI_FAST, 0,
          TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
#ifdef JOY_TYPE_WINGWARRIOR
        { "Wing_man Warrior",
          "Wingman Warrior",
          joy_hw_callback, (void *)JOY_TYPE_WINGWARRIOR, 0,
          TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
    { NULL }
};


static tui_menu_item_def_t single_joystick_submenu[] = {
    { "Joystick _Device:",
      "Specify device for joystick emulation",
      get_joystick_device_callback, (void *)1, 19,
      TUI_MENU_BEH_CONTINUE, joy_device_1_submenu, "Joystick" },
    { "--" },
    { "Configure Keyset _A...",
      "Configure keyboard set A for joystick emulation",
      NULL, NULL, 0,
      TUI_MENU_BEH_CONTINUE, keyset_1_submenu, "Keyset A" },
    { "Configure Keyset _B...",
      "Configure keyboard set B for joystick emulation",
      NULL, NULL, 0,
      TUI_MENU_BEH_CONTINUE, keyset_2_submenu, "Keyset B" },
    { "--" },
    { "Set joystick _hardware type",
      "Set type of PC joystick(s)",
      get_hw_joystick_type_callback, NULL, 21,
      TUI_MENU_BEH_CONTINUE, joy_list_submenu, "List of joystick types" },
    { NULL }
};

static tui_menu_item_def_t double_joystick_submenu[] = {
    { "_Swap",
      "Swap joystick ports",
      swap_joysticks_callback, NULL, 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "--" },
    { "Port #_1:",
      "Specify device for emulation of joystick in port #1",
      get_joystick_device_callback, (void *)1, 19,
      TUI_MENU_BEH_CONTINUE, joy_device_1_submenu, "Joystick #1" },
    { "Port #_2:",
      "Specify device for emulation of joystick in port #2",
      get_joystick_device_callback, (void *)2, 19,
      TUI_MENU_BEH_CONTINUE, joy_device_2_submenu, "Joystick #2" },
    { "--" },
    { "Configure Keyset _A...",
      "Configure keyboard set A for joystick emulation",
      NULL, NULL, 0,
      TUI_MENU_BEH_CONTINUE, keyset_1_submenu, "Keyset A" },
    { "Configure Keyset _B...",
      "Configure keyboard set B for joystick emulation",
      NULL, NULL, 0,
      TUI_MENU_BEH_CONTINUE, keyset_2_submenu, "Keyset B" },
    { "--" },
    { "Set joystick _hardware type",
      "Set type of PC joystick(s)",
      get_hw_joystick_type_callback, NULL, 30,
      TUI_MENU_BEH_CONTINUE, joy_list_submenu, "List of joystick types" },
    { NULL }
};

void uijoystick_init(struct tui_menu *parent_submenu,
                     unsigned int number_joysticks)
{
    tui_menu_t ui_joystick_settings_submenu;

    if (number_joysticks > 0) {
        ui_joystick_settings_submenu = tui_menu_create("Joystick Settings", 1);
        tui_menu_add_submenu(parent_submenu, "_Joystick Settings...",
                             "Joystick settings",
                             ui_joystick_settings_submenu, NULL, 0,
                             TUI_MENU_BEH_CONTINUE);
        if (number_joysticks == 2)
            tui_menu_add(ui_joystick_settings_submenu,
                         double_joystick_submenu);
        else                    /* Just one joystick.  */
            tui_menu_add(ui_joystick_settings_submenu,
                         single_joystick_submenu);
    }
}

