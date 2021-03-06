/*
 * video-viewport.c
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

#include "lib.h"
#include "machine.h"
#include "video.h"
#include "videoarch.h"
#include "viewport.h"


void video_viewport_get(video_canvas_t *canvas, viewport_t **viewport,
                        geometry_t **geometry)
{
    *viewport = canvas->viewport;
    *geometry = canvas->geometry;
    return;
}

void video_viewport_resize(video_canvas_t *canvas)
{
    geometry_t *geometry;
    viewport_t *viewport;
    rectangle_t *screen_size;
    rectangle_t *gfx_size;
    position_t *gfx_position;
    unsigned int gfx_height;
    unsigned width, height;

    if (canvas->initialized == 0)
        return;

    geometry = canvas->geometry;
    viewport = canvas->viewport;

    screen_size = &geometry->screen_size;
    gfx_size = &geometry->gfx_size;
    gfx_position = &geometry->gfx_position;

    width = canvas->draw_buffer->canvas_width;
    height = canvas->draw_buffer->canvas_height;

    if (width >= screen_size->width) {
        viewport->x_offset = (width - screen_size->width) / 2;
        viewport->first_x = 0;
    } else {
        viewport->x_offset = 0;

        if (geometry->gfx_area_moves) {
            viewport->first_x = (screen_size->width - width) / 2;
        } else {
            viewport->first_x = gfx_position->x;

            if (width > gfx_size->width)
                viewport->first_x -= (width - gfx_size->width) / 2;
        }
    }

    gfx_height = height - gfx_size->height;

    if (height >= screen_size->height) {
        viewport->y_offset = (height - screen_size->height) / 2;
        viewport->first_line = 0;
        viewport->last_line = screen_size->height - 1;
    } else {
        viewport->y_offset = 0;

        if (geometry->gfx_area_moves) {
            viewport->first_line = (screen_size->height - height) / 2;
        } else {
            /* FIXME: Somewhat buggy.  */
            viewport->first_line = gfx_position->y;

            if (height > gfx_size->height) {
                if (gfx_height <= gfx_position->y)
                    viewport->first_line -= gfx_height / 2;
                else
                    viewport->first_line = 0;
            }

        }
        viewport->last_line = (viewport->first_line + height) - 1;
    }

    if (!vsid_mode && !console_mode)
        video_canvas_resize(canvas, width, height);

    /* Make sure we don't waste space showing unused lines.  */
    if ((viewport->first_line < geometry->first_displayed_line
        && viewport->last_line < geometry->last_displayed_line)
        || (viewport->first_line > geometry->first_displayed_line
        && viewport->last_line > geometry->last_displayed_line)) {
        viewport->first_line = geometry->first_displayed_line;
        viewport->last_line = (geometry->first_displayed_line + height - 1);
    }
    video_canvas_refresh_all(canvas);
}

void video_viewport_title_set(video_canvas_t *canvas, const char *title)
{
    viewport_t *viewport;

    viewport = canvas->viewport;

    lib_free(viewport->title);
    viewport->title = lib_stralloc(title);
}

void video_viewport_title_free(viewport_t *viewport)
{
    lib_free(viewport->title);
}

