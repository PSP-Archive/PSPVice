/*
 * video.c
 *
 * Written by
 *  Mike Dawson <mike@gp2x.org>
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
#include "minimal.h"
#include "ui_gp2x.h"
#include "uitext_gp2x.h"
#include "prefs_gp2x.h"
#include "vkeyboard_gp2x.h"

#include "videoarch.h"
#include "palette.h"
#include "machine.h"

int machine_ui_done=0;
int vicii_setup_delay=0;

extern unsigned short *gp2x_memregs;

video_canvas_t *current_canvas;

int xoffset, yoffset;
int xoffset_centred, yoffset_centred;
int xoffset_uncentred, yoffset_uncentred;

int video_init() {
	return 0;
}

int video_init_cmdline_options() {
	return 0;
}

void video_shutdown() {
}

video_canvas_t *video_canvas_create(video_canvas_t *canvas, 
		unsigned int *width, unsigned int *height, int mapped) {
	static int vicii_setup=0;

	display_width=320;

	canvas->depth=8;
	canvas->width=320;
	canvas->height=240;

	video_canvas_set_palette(canvas, canvas->palette);

	if (vicii_setup_delay==1)
	{
		if (!vicii_setup) canvas->width=0;
		vicii_setup=1;
	}

	tvout_pal=1;
	if(gp2x_memregs[0x2800>>1]&0x100) {
		tvout=1;
		hwscaling=1;
#if 0
		tvout_pal=gp2x_memregs[0x2800>>1]&0x1000;
#endif
		if(gp2x_memregs[0x2818>>1]==239) tvout_pal=0;
		printf("\n0x2818=%d\n", gp2x_memregs[0x2818>>1]);
		display_set();
	}

	current_canvas=canvas;
	return canvas;
}

void video_canvas_destroy(struct video_canvas_s *canvas) {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void video_arch_canvas_init(struct video_canvas_s *canvas) {
	canvas->video_draw_buffer_callback=NULL;
}

void video_canvas_refresh(struct video_canvas_s *canvas,
                                 unsigned int xs, unsigned int ys,
                                 unsigned int xi, unsigned int yi,
                                 unsigned int w, unsigned int h) {
#if 0
	fprintf(stderr, "calling %s\n", __func__);
	fprintf(stderr, "refresh xs=%d, ys=%d, xi=%d, yi=%d, w=%d, h=%d\n",
			xs, ys, xi, yi, w, h);
#endif
	BYTE *source;
	register int x, y;
	register int buf_width;
#if 0
	register BYTE pixel;
#endif
	register int xoff;
	register int yoff;

	if(canvas->width==0) return;

	if (machine_ui_done==0) return;

      if (xoffset_centred==0)
	{
		xoffset_centred=(canvas->draw_buffer->draw_buffer_width-320)/2;
		xoffset=xoffset_centred;
	}
	if (yoffset_centred==0)
	{
		yoffset_centred=(canvas->draw_buffer->draw_buffer_height-240)/2;
		yoffset=xoffset_centred;
	}

	xoff=xoffset;
	yoff=yoffset;

	source=canvas->draw_buffer->draw_buffer;
	buf_width=canvas->draw_buffer->draw_buffer_width;

	register unsigned long *screen32=(unsigned long *)gp2x_screen8;
	register unsigned long *source32=(unsigned long *)source;

	if(hwscaling) {
		for(y=0; y<272; y++) {
			for(x=0; x<384/4; x++) {
				screen32[(y*(384/4))+x]=
					source32[((y+(yoff-16))*(buf_width/4))+x+((xoff-32)/4)];
			}
		}
	} else {
		for(y=0; y<240; y++) {
			for(x=0; x<320/4; x++) {
				screen32[(y*(320/4))+x]=
					source32[((y+yoff)*(buf_width/4))+x+(xoff/4)];
			}
		}
	}

	gp2x_poll_input();
	if(stats_open) draw_stats(gp2x_screen8);
	if(prefs_open) draw_prefs(gp2x_screen8);
	else if(vkeyb_open) draw_vkeyb(gp2x_screen8);
	
	gp2x_video_flip();
}

int video_canvas_set_palette(struct video_canvas_s *canvas,
                                    struct palette_s *palette) {
	fprintf(stderr, "calling %s\n", __func__);
	int i;
	for(i=0; i<palette->num_entries; i++) {
		gp2x_palette[i*2]=
			((palette->entries[i].green)<<8)
			| (palette->entries[i].blue);
		gp2x_palette[i*2+1]=palette->entries[i].red;
	}
	gp2x_video_setpalette();
}

int video_arch_resources_init() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void video_canvas_resize(struct video_canvas_s *canvas,
                                unsigned int width, unsigned int height) {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void video_arch_resources_shutdown() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void video_add_handlers() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void fullscreen_capability() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}
