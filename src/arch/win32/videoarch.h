/*
 * video.h - Video implementation for Win32, using DirectDraw.
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

#ifndef _VIDEOARCH_H
#define _VIDEOARCH_H

/*  Nasty patch to avoid defining BYTE, WORD, DWORD more than once.  */

#undef BYTE
#undef WORD
#undef DWORD
#include <ddraw.h>

#include "types.h"
#include "video.h"

#define CANVAS_USES_TRIPLE_BUFFERING(c) 0

struct palette_s;
struct video_draw_buffer_callback_s;
struct video_resource_chip_s;

typedef struct video_canvas_s {
    unsigned int initialized;
    unsigned int created;
    char *title;
    int width, height;
    int mapped;
    int depth;
    float refreshrate; /* currently displayed refresh rate */
    struct video_render_config_s *videoconfig;
    struct draw_buffer_s *draw_buffer;
    struct viewport_s *viewport;
    struct geometry_s *geometry;
    struct palette_s *palette;
    struct video_resource_chip_s *video_resource_chip;
    BYTE *pixels;
    HWND hwnd;
    LPDIRECTDRAW        dd_object;
    LPDIRECTDRAW2       dd_object2;
    LPDIRECTDRAWSURFACE render_surface;
    LPDIRECTDRAWSURFACE primary_surface;
    LPDIRECTDRAWSURFACE back_surface;
    LPDIRECTDRAWSURFACE temporary_surface;
    LPDIRECTDRAWCLIPPER clipper;
    LPDIRECTDRAWPALETTE dd_palette;
    int client_width;
    int client_height;
    struct video_draw_buffer_callback_s *video_draw_buffer_callback;
} video_canvas_t;

/* ------------------------------------------------------------------------ */

const char *dd_error(HRESULT ddrval);

extern int video_set_palette(video_canvas_t *c);
extern int video_set_physical_colors(video_canvas_t *c);

extern int video_create_triple_surface(struct video_canvas_s *canvas,
                                       int width, int height);
extern int video_create_single_surface(struct video_canvas_s *canvas,
                                       int width, int height);

extern video_canvas_t *video_canvas_for_hwnd(HWND hwnd);

extern void video_canvas_update(HWND hwnd, HDC hdc, int xclient, int yclient,
                                int w, int h);

extern float video_refresh_rate(video_canvas_t *c);

#endif

