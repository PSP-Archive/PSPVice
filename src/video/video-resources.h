/*
 * video-resources.h - Resources for the video output
 *
 * Written by
 *  John Selck <graham@cruise.de>
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

#ifndef _VIDEO_RESOURCES_H
#define _VIDEO_RESOURCES_H

#define VIDEO_RESOURCE_PAL_MODE_FAST	0
#define VIDEO_RESOURCE_PAL_MODE_TRUE	1

struct video_resources_s
{
    int color_saturation;
    int color_contrast;
    int color_brightness;
    int color_gamma;
    int delayloop_emulation;	/* fake PAL emu */
    int pal_scanlineshade;
    int pal_blur;
    int pal_mode;
};

typedef struct video_resources_s video_resources_t;

extern video_resources_t video_resources;

#endif

