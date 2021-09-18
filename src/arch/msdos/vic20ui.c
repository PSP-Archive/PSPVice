/*
 * vic20ui.c - Definition of the VIC20-specific part of the UI.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cartridge.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "menudefs.h"
#include "resources.h"
#include "tui.h"
#include "tuimenu.h"
#include "tuifs.h"
#include "types.h"
#include "ui.h"
#include "uisidcart.h"
#include "util.h"
#include "vic20ui.h"


static TUI_MENU_CALLBACK(toggle_MachineVideoStandard_callback)
{
    int value;

    resources_get_int("MachineVideoStandard", &value);

    if (been_activated) {
	    if (value == MACHINE_SYNC_PAL)
	        value = MACHINE_SYNC_NTSC;
        else
	        value = MACHINE_SYNC_PAL;

        resources_set_int("MachineVideoStandard", value);
    }

    switch (value) {
      case MACHINE_SYNC_PAL:
	return "PAL-G";
      case MACHINE_SYNC_NTSC:
	return "NTSC-M";
      default:
	return "(Custom)";
    }
}

TUI_MENU_DEFINE_TOGGLE(VICVideoCache)
TUI_MENU_DEFINE_TOGGLE(PALEmulation)

static TUI_MENU_CALLBACK(toggle_PALMode_callback)
{
    int value;

    resources_get_int("PALMode", &value);

    if (been_activated) {
        value = (value + 1) % 3;
        resources_set_int("PALMode", value);
    }

    switch (value) {
      case 0:
	return "Fast PAL";
      case 1:
	return "Y/C cable (sharp)";
      case 2:
	return "Composite (blurry)";
      default:
	return "unknown";
    }
}

static tui_menu_item_def_t vic_menu_items[] = {
    { "Video _Cache:",
      "Enable screen cache (disabled when using triple buffering)",
      toggle_VICVideoCache_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "_PAL Emulation:",
      "Enable PAL emulation",
      toggle_PALEmulation_callback, NULL, 3,
      TUI_MENU_BEH_RESUME, NULL, NULL },
    { "PAL _Mode:",
      "Change PAL Mode",
      toggle_PALMode_callback, NULL, 20,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "--" },
    { "V_ideo Standard:",
      "Select machine clock ratio",
      toggle_MachineVideoStandard_callback, NULL, 11,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static WORD cartridge_type_to_address(int type)
{
    /* We might use a simple AND here, but it's safer to use `switch()' as
       speed does not matter in this case.  */
    switch (type) {
      case CARTRIDGE_VIC20_4KB_2000:
      case CARTRIDGE_VIC20_8KB_2000:
      case CARTRIDGE_VIC20_16KB_2000:
        return 0x2000;
      case CARTRIDGE_VIC20_4KB_4000:
      case CARTRIDGE_VIC20_8KB_4000:
      case CARTRIDGE_VIC20_16KB_4000:
	return 0x4000;
      case CARTRIDGE_VIC20_4KB_6000:
      case CARTRIDGE_VIC20_8KB_6000:
      case CARTRIDGE_VIC20_16KB_6000:
        return 0x6000;
      case CARTRIDGE_VIC20_4KB_A000:
      case CARTRIDGE_VIC20_8KB_A000:
        return 0xa000;
      case CARTRIDGE_VIC20_4KB_B000:
        return 0xb000;
      case CARTRIDGE_VIC20_DETECT:
      default:
        return 0;               /* bogus */
    }
}

static TUI_MENU_CALLBACK(attach_cartridge_callback)
{
    const char *s;
    int type = (int)param;

    if (been_activated) {
        char *default_item, *directory;
        char *name;

        s = cartridge_get_file_name(cartridge_type_to_address(type));
        if (s == NULL)
            directory = default_item = NULL;
        else
            util_fname_split(s, &directory, &default_item);

        name = tui_file_selector("Attach cartridge image",
                                 directory, "*", default_item, 0, NULL, 0,
                                 NULL, NULL);
        if (name != NULL
            && (s == NULL || strcasecmp(name, s) != 0)
            && cartridge_attach_image(type, name) < 0)
            tui_error("Invalid cartridge image.");
        ui_update_menus();
        lib_free(name);
    }

    /* This is redundant if `been_activated' is nonzero, but let's stay on
       the safe side.  */
    s = cartridge_get_file_name(cartridge_type_to_address(type));
    if (s == NULL || *s == '\0')
        return "(none)";
    else
        return s;
}

static tui_menu_item_def_t attach_cartridge_menu_items[] = {
    { "--" },
    { "Cartridge at $_2000:",
      "Attach a cartridge image at address $2000",
      attach_cartridge_callback, (void *)CARTRIDGE_VIC20_16KB_2000, 30,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Cartridge at $_4000:",
      "Attach a cartridge image at address $4000",
      attach_cartridge_callback, (void *)CARTRIDGE_VIC20_16KB_4000, 30,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Cartridge at $_6000:",
      "Attach a cartridge image at address $6000",
      attach_cartridge_callback, (void *)CARTRIDGE_VIC20_16KB_6000, 30,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Cartridge at $_A000:",
      "Attach a cartridge image at address $A000",
      attach_cartridge_callback, (void *)CARTRIDGE_VIC20_8KB_A000, 30,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Cartridge at $_B000:",
      "Attach a cartridge image at address $B000",
      attach_cartridge_callback, (void *)CARTRIDGE_VIC20_4KB_B000, 30,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};

static TUI_MENU_CALLBACK(detach_cartridge_callback)
{
    if (been_activated)
        cartridge_detach_image();

    return NULL;
}

static tui_menu_item_def_t detach_cartridge_menu_items[] = {
    { "--" },
    { "_Cartridges",
      "Detach all attached cartridge images",
      detach_cartridge_callback, NULL, 30,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

enum {
    MEM_NONE,
    MEM_ALL,
    MEM_3K,
    MEM_8K,
    MEM_16K,
    MEM_24K
};

enum {
    BLOCK_0 = 1,
    BLOCK_1 = 1 << 1,
    BLOCK_2 = 1 << 2,
    BLOCK_3 = 1 << 3,
    BLOCK_5 = 1 << 5
};

static TUI_MENU_CALLBACK(set_common_memory_configuration_callback)
{
    if (been_activated) {
        int blocks;

        switch ((int)param) {
          case MEM_NONE:
            blocks = 0;
            break;
          case MEM_ALL:
            blocks = BLOCK_0 | BLOCK_1 | BLOCK_2 | BLOCK_3 | BLOCK_5;
            break;
          case MEM_3K:
            blocks = BLOCK_0;
            break;
          case MEM_8K:
            blocks = BLOCK_1;
            break;
          case MEM_16K:
            blocks = BLOCK_1 | BLOCK_2;
            break;
          case MEM_24K:
            blocks = BLOCK_1 | BLOCK_2 | BLOCK_3;
            break;
          default:
            /* Shouldn't happen.  */
            log_debug("What?!");
            blocks = 0;         /* Make compiler happy.  */
        }
        resources_set_int("RamBlock0", blocks & BLOCK_0 ? 1 : 0);
        resources_set_int("RamBlock1", blocks & BLOCK_1 ? 1 : 0);
        resources_set_int("RamBlock2", blocks & BLOCK_2 ? 1 : 0);
        resources_set_int("RamBlock3", blocks & BLOCK_3 ? 1 : 0);
        resources_set_int("RamBlock5", blocks & BLOCK_5 ? 1 : 0);
        ui_update_menus();
    }

    /* This way, the "Not Really!" item is always the default one.  */
    *become_default = 0;
    return NULL;
}

static tui_menu_item_def_t common_memory_configurations_items[] = {
    { "_Not Really!",
      "Keep the current settings",
      NULL, NULL, 0, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "--" },
    { "_Unexpanded",
      "Setup a completely unexpanded VIC20",
      set_common_memory_configuration_callback, (void *)MEM_NONE, NULL,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_3K (block 0)",
      "Setup a 3K-expanded VIC20",
      set_common_memory_configuration_callback, (void *)MEM_3K, NULL,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_8K (block 1)",
      "Setup an 8K-expanded VIC20",
      set_common_memory_configuration_callback, (void *)MEM_8K, NULL,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_16K (blocks 1/2)",
      "Setup an 8K-expanded VIC20",
      set_common_memory_configuration_callback, (void *)MEM_16K, NULL,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_24K (blocks 1/2/3)",
      "Setup a 24K-expanded VIC20",
      set_common_memory_configuration_callback, (void *)MEM_24K, NULL,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_All (blocks 0/1/2/3/5)",
      "Setup a VIC20 with all the possible RAM stuffed in",
      set_common_memory_configuration_callback, (void *)MEM_ALL, NULL,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

TUI_MENU_DEFINE_TOGGLE(RAMBlock0)
TUI_MENU_DEFINE_TOGGLE(RAMBlock1)
TUI_MENU_DEFINE_TOGGLE(RAMBlock2)
TUI_MENU_DEFINE_TOGGLE(RAMBlock3)
TUI_MENU_DEFINE_TOGGLE(RAMBlock5)

TUI_MENU_DEFINE_TOGGLE(EmuID)

static tui_menu_item_def_t special_menu_items[] = {
    { "_Emulator Identification:",
      "Allow programs to identify the emulator they are running on",
      toggle_EmuID_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "--" },
    { "Choose Common _Memory Configuration...",
      "Choose memory configuration from a set of common ones.",
      NULL, NULL, 0,
      TUI_MENU_BEH_CONTINUE, common_memory_configurations_items,
      "Setup Memory Configuration" },
    { "  RAM Block _0 (3K at $0400-$0F00)",
      "Enable RAM expansion block at address $0400-$0F00",
      toggle_RAMBlock0_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "  RAM Block _1 (8K at $2000-$3FFF)",
      "Enable RAM expansion block at address $2000-$3FFF",
      toggle_RAMBlock1_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "  RAM Block _2 (8K at $4000-$5FFF)",
      "Enable RAM expansion block at address $4000-$5FFF",
      toggle_RAMBlock2_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "  RAM Block _3 (8K at $6000-$7FFF)",
      "Enable RAM expansion block at address $6000-$7FFF",
      toggle_RAMBlock3_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "  RAM Block _5 (8K at $A000-$BFFF)",
      "Enable RAM expansion block at address $A000-$BFFF",
      toggle_RAMBlock5_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static TUI_MENU_CALLBACK(load_rom_file_callback)
{
    if (been_activated) {
        char *name;

        name = tui_file_selector("Load ROM file",
                                 NULL, "*", NULL, 0, NULL, 0, NULL, NULL);

        if (name != NULL) {
            if (resources_set_string(param, name) < 0)
                ui_error("Could not load ROM file '%s'", name);
            lib_free(name);
        }
    }
    return NULL;
}

static tui_menu_item_def_t rom_menu_items[] = {
    { "--" },
    { "Load new _Kernal ROM...",
      "Load new Kernal ROM",
      load_rom_file_callback, "KernalName", 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Load new _BASIC ROM...",
      "Load new BASIC ROM",
      load_rom_file_callback, "BasicName", 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Load new _Character ROM...",
      "Load new Character ROM",
      load_rom_file_callback, "ChargenName", 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Load new 15_41 ROM...",
      "Load new 1541 ROM",
      load_rom_file_callback, "DosName1541", 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Load new 1541-_II ROM...",
      "Load new 1541-II ROM",
      load_rom_file_callback, "DosName1541ii", 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Load new 15_71 ROM...",
      "Load new 1571 ROM",
      load_rom_file_callback, "DosName1571", 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Load new 15_81 ROM...",
      "Load new 1581 ROM",
      load_rom_file_callback, "DosName1581", 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Load new _2031 ROM...",
      "Load new 2031 ROM",
      load_rom_file_callback, "DosName2031", 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Load new _1001 ROM...",
      "Load new 1001 ROM",
      load_rom_file_callback, "DosName1001", 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

int vic20ui_init(void)
{
    ui_create_main_menu(1, 1, 1, 1, 1);

    tui_menu_add(ui_attach_submenu, attach_cartridge_menu_items);
    tui_menu_add(ui_detach_submenu, detach_cartridge_menu_items);
    tui_menu_add(ui_special_submenu, special_menu_items);

    uisidcart_init(ui_special_submenu, "$9800", "$9C00", "VIC20");

    tui_menu_add(ui_rom_submenu, rom_menu_items);
    tui_menu_add(ui_video_submenu, vic_menu_items);

    return 0;
}

void vic20ui_shutdown(void)
{
}

