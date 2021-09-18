/*
 * video.c - PS2 graphics handling
 *
 * Written by
 *  Rami Räsänen <raipsu@users.sourceforge.net>
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

#include "types.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "cmdline.h"
#include "kbd.h"
#include "lib.h"
#include "log.h"
#include "palette.h"
#include "resources.h"
#include "types.h"
#include "ui.h"
#include "util.h"
#include "video.h"
#include "videoarch.h"

#define MAX_CANVAS_NUM 2

static video_canvas_t *canvaslist[MAX_CANVAS_NUM];

unsigned char* ps2_videobuf = (BYTE*) NULL;

int video_arch_resources_init(void)
{
  return 0;
}

void video_arch_resources_shutdown(void)
{
}

int video_init_cmdline_options(void)
{
  return 0;
}

int video_init(void)
{
    int i;

    for (i = 0; i < MAX_CANVAS_NUM; i++)
        canvaslist[i] = NULL;

    ps2_videobuf = (BYTE*) memalign(32, 512*512*4); // 640x400

    return 0;
}

void video_shutdown(void)
{
}

void video_arch_canvas_init(struct video_canvas_s *canvas)
{
  canvas->video_draw_buffer_callback = NULL;
}

video_canvas_t* video_canvas_create(video_canvas_t* canvas, unsigned int *width, unsigned int *height, int mapped)
{
    int i;
    int next_canvas = 0;

    printf("Video Canvas create");

    if (canvas->palette->num_entries > NUM_AVAILABLE_COLORS)
    {
        return NULL;
    }

    printf ("create foobar %d %d\n", *width, *height);

    video_canvas_resize(canvas, *width, *height);
    *width = canvas->width;
    *height = canvas->height;

    video_canvas_set_palette(canvas, canvas->palette);

    canvas->back_page = 1;

    while (canvaslist[next_canvas] != NULL && next_canvas < MAX_CANVAS_NUM - 1)
        next_canvas++;
    canvaslist[next_canvas] = canvas;


    for (i = 0; i < canvas->palette->num_entries; i++)
    {
        video_render_setphysicalcolor(canvas->videoconfig, i, 0x7F000000 | (canvas->palette->entries[i].blue<<16) |
                                                         (canvas->palette->entries[i].green<<8) |
                                                         (canvas->palette->entries[i].red), canvas->depth);
    }

    return canvas;
}

void video_canvas_destroy(video_canvas_t *canvas)
{
    int i;

    if (canvas == NULL)
        return;

    video_canvas_shutdown(canvas);

    for (i = 0; i < MAX_CANVAS_NUM; i++)
        if (canvaslist[i] == canvas)
            canvaslist[i] = NULL;
}

int video_canvas_set_palette(struct video_canvas_s *canvas, palette_t *palette)
{
    canvas->palette = palette;

    return 0;
}

void video_canvas_map(video_canvas_t *canvas)
{
    /* Not implemented. */
}

void video_canvas_unmap(video_canvas_t *canvas)
{
    /* Not implemented. */
}

void video_canvas_resize(video_canvas_t *canvas, unsigned int width, unsigned int height)
{
/*
    if (canvas->videoconfig->doublesizex)
        width *= 2;

    if (canvas->videoconfig->doublesizey)
        height *= 2;
*/

    canvas->width = width;
    canvas->height = height;

    //printf ("width = %d, height = %d\n", width, height);

    /*  pal c64 = 544 x 272  (c128 samat) */
    /* ntsc c64 = 454 x 227 */
    /*  pal vic = 568 x 284 */
    /* ntsc vic = 506 x 253 */
    /*  pal +4  = 576 x 288 */
    /* ntsc +4  = 454 x 227 */
    /*  pal 128 = 384 x 272 */
    /* ntsc 128 = 384 x 227 */

    // CK TODO
    /*
    if (height == 284)
      g2_init(PAL_VIC20);
    else if (height == 253)
      g2_init(NTSC_VIC20);
    else if (height == 272)
      g2_init(PAL_C64);
    else if (height == 227)
      g2_init(NTSC_C64);
    else if (height == 282)
      g2_init(PAL_PET);
    else
      g2_init(PAL_C64);

    g2_set_visible_frame(1);
    g2_set_active_frame(0);

    g2_set_fill_color(0, 0, 0);
    g2_fill_rect(0, 0, g2_get_max_x(), g2_get_max_y());
    g2_flip_buffers();
    g2_fill_rect(0, 0, g2_get_max_x(), g2_get_max_y());
    */

    //canvas->depth = 32;
    canvas->depth = 8;
    canvas->bytes_per_line = canvas->width * canvas->depth / 8;
}

/* ------------------------------------------------------------------------- */

void video_canvas_refresh(video_canvas_t *c,
                                 unsigned int xs, unsigned int ys,
                                 unsigned int xi, unsigned int yi,
                                 unsigned int w, unsigned int h)
{
    //printf("video_canvas_refresh w=%d h=%d",w,h);

    video_canvas_render(c,
                        (BYTE *)(ps2_videobuf),
                        w, h,
                        xs, ys,
                        xi, yi,
                        c->bytes_per_line,
                        c->depth);

    return;
}


void fullscreen_capability(cap_fullscreen_t *cap_fullscreen)
{
    cap_fullscreen->device_num = 0;
}
