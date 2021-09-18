/*
 * x11ui.h
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#ifndef _X11UI_H
#define _X11UI_H

#include "vice.h"

#ifdef USE_GNOMEUI
#include "x11/gnome/uiarch.h"
#else
#include "x11/xaw/uiarch.h"
#endif

extern int x11ui_open_canvas_window(struct video_canvas_s *c,
                                    const char *title, int width,
                                    int height, int no_autorepeat);
extern void x11ui_resize_canvas_window(ui_window_t w, int height, int width, int hwscale);
extern void x11ui_move_canvas_window(ui_window_t w, int x, int y);
extern void x11ui_canvas_position(ui_window_t w, int *x, int *y);
extern void x11ui_get_widget_size(ui_window_t win, int *w, int *h);
extern void x11ui_map_canvas_window(ui_window_t w);
extern void x11ui_unmap_canvas_window(ui_window_t w);
extern void x11ui_destroy_widget(ui_window_t w);

extern Display *x11ui_get_display_ptr(void);
extern Window x11ui_get_X11_window(void);
extern int x11ui_get_display_depth(void);
extern int x11ui_get_screen(void);
extern void x11ui_fullscreen(int enable);

#endif

