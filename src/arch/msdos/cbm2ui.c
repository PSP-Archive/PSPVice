/*
 * cbm2ui.c - Definition of the CBM2-specific part of the UI.
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

#include "cbm2ui.h"
#include "menudefs.h"
#include "lib.h"
#include "resources.h"
#include "tui.h"
#include "tuifs.h"
#include "ui.h"
#include "uisid.h"


static struct {
    char *name;
    char *brief_description;
    const char *menu_item;
    char *long_description;
} palette_items[] = {
    { "default", "Default (green)", "_Default",
      "Default VICE C610 palette (Green)" },
    { "amber", "Amber", "_Amber",
      "Amber palette" },
    { "white", "White", "_White",
      "White palette" },
    { NULL }
};

static TUI_MENU_CALLBACK(palette_callback)
{
    if (been_activated) {
        if (resources_set_string("CrtcPaletteFile", (const char *)param) < 0)
            tui_error("Invalid palette file");
        ui_update_menus();
    }

    return NULL;
}

static TUI_MENU_CALLBACK(custom_palette_callback)
{
    if (been_activated) {
        char *name;

        name = tui_file_selector("Load custom palette",
                                 NULL, "*.vpl", NULL, 0, NULL, 0, NULL, NULL);

        if (name != NULL) {
            if (resources_set_string("CrtcPaletteFile", name) < 0)
                tui_error("Invalid palette file");
            ui_update_menus();
            lib_free(name);
        }
    }
    return NULL;
}

static TUI_MENU_CALLBACK(palette_menu_callback)
{
    const char *s;
    int i;

    resources_get_string("CrtcPaletteFile", &s);
    for (i = 0; palette_items[i].name != NULL; i++) {
        if (strcmp(s, palette_items[i].name) == 0)
           return palette_items[i].brief_description;
    }

    return "Custom";
}

static void add_palette_submenu(tui_menu_t parent)
{
    int i;
    tui_menu_t palette_menu = tui_menu_create("Color Set", 1);

    for (i = 0; palette_items[i].name != NULL; i++)
        tui_menu_add_item(palette_menu,
                          palette_items[i].menu_item,
                          palette_items[i].long_description,
                          palette_callback,
                          (void *) palette_items[i].name, 0,
                          TUI_MENU_BEH_RESUME);

    tui_menu_add_item(palette_menu,
                      "C_ustom",
                      "Load a custom palette",
                      custom_palette_callback,
                      NULL, 0,
                      TUI_MENU_BEH_RESUME);

    tui_menu_add_submenu(parent, "Color _Palette:",
			 "Choose color palette",
			 palette_menu,
			 palette_menu_callback,
			 NULL,
			 15);
}

/* ------------------------------------------------------------------------- */

TUI_MENU_DEFINE_TOGGLE(CrtcDoubleScan)
TUI_MENU_DEFINE_TOGGLE(CrtcVideoCache)

static tui_menu_item_def_t video_menu_items[] = {
    { "Video _Cache:",
      "Enable screen cache (disabled when using triple buffering)",
      toggle_CrtcVideoCache_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "--" },
    { "_Double Scan:",
      "Display double lines when running in 80-column mode",
      toggle_CrtcDoubleScan_callback, NULL, 3,
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

int cbm2ui_init(void)
{
    ui_create_main_menu(0, 1, 0, 0, 1);

    tui_menu_add_separator(ui_video_submenu);
    tui_menu_add(ui_sound_submenu, sid_ui_menu_items);

    add_palette_submenu(ui_video_submenu);

    tui_menu_add(ui_video_submenu, video_menu_items);
    tui_menu_add(ui_rom_submenu, rom_menu_items);

    return 0;
}

void cbm2ui_shutdown(void)
{
}

