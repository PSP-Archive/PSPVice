/*
 * ui.h - Windows user interface.
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

#ifndef _UI_WIN32_H
#define _UI_WIN32_H

#include "types.h"
#include "uiapi.h"

#include <stdio.h>

#undef BYTE
#undef WORD
#undef DWORD
#include <ddraw.h>

#define APPLICATION_CLASS TEXT("VICE")
#define APPLICATION_CLASS_MAIN TEXT("VICE:Main")
#define APPLICATION_FULLSCREEN_CLASS TEXT("VICE:Fullscreen")
#define CONSOLE_CLASS "VICE:Console"
#define MONITOR_CLASS "VICE:Monitor"

typedef enum {
    UI_BUTTON_NONE, UI_BUTTON_CLOSE, UI_BUTTON_OK, UI_BUTTON_CANCEL,
    UI_BUTTON_YES, UI_BUTTON_NO, UI_BUTTON_RESET, UI_BUTTON_HARDRESET,
    UI_BUTTON_MON, UI_BUTTON_DEBUG, UI_BUTTON_CONTENTS, UI_BUTTON_AUTOSTART
} ui_button_t;

struct ui_menu_toggle_s {
    /* Name of resource.  */
    const char *name;
    /* ID of the corresponding menu item.  */
    UINT item_id;
};
typedef struct ui_menu_toggle_s ui_menu_toggle_t;

struct ui_res_possible_values_s {
    int value;
    UINT item_id; /* The last item_id has to be zero.  */
};
typedef struct ui_res_possible_values_s ui_res_possible_values_t;

struct ui_res_value_list_s {
    const char *name;
    const ui_res_possible_values_t *vals;
    UINT default_item_id;
};
typedef struct ui_res_value_list_s ui_res_value_list_t;

extern int  ui_active;
extern HWND ui_active_window;

/* ------------------------------------------------------------------------- */

extern int ui_vblank_sync_enabled();

extern void ui_exit(void);
extern void ui_display_speed(float percent, float framerate, int warp_flag);
extern void ui_display_paused(int flag);
extern void ui_dispatch_next_event(void);
extern void ui_dispatch_events(void);
extern void ui_error_string(const char *text);

extern ui_button_t ui_ask_confirmation(const char *title, const char *text);

typedef void (*ui_machine_specific_t) (WPARAM wparam, HWND hwnd);

extern void ui_register_machine_specific(ui_machine_specific_t func);
extern void ui_register_menu_toggles(const ui_menu_toggle_t *toggles);
extern void ui_register_res_values(const ui_res_value_list_t *valuelist);

/* ------------------------------------------------------------------------ */

/* Windows-specific functions.  */

extern void ui_update_menu();
extern HWND ui_get_main_hwnd(void);
extern HWND ui_open_canvas_window(const char *title, unsigned int width,
                                  unsigned int height,
                                  int fullscreen);
extern void ui_resize_canvas_window(HWND w, unsigned int width,
                                    unsigned int height);
extern FILE *ui_console_save_dialog(HWND hwnd);
extern int ui_emulation_is_paused(void);
extern void ui_set_alwaysontop(int alwaysontop);

#endif

