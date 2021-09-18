/*
 * soundps2.c - Implementation of the PS2 sound device
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

#include <stdio.h>

#include "sound.h"
#include "../arch/psp/pspaudio.h"


extern const int num_audio_buffs;
extern const int audio_buff_size;

extern int sample_rate_div;

#define FRAG 735		//	44100/60
//#define FRAG 882		//	44100/50
#define FRAGNR 16

extern int write_offset;

static int pspsound_init(const char *param, int *speed,int *fragsize, int *fragnr, int *channels)
{
    /* No stereo available. */
    *channels = 1;
    *fragsize = FRAG;
    *fragnr = FRAGNR;

	*speed = 44100;

	pspaudio_init();

    return 0;
}

static int pspsound_write(SWORD *pbuf, size_t nr)
{
	pspaudio_write(pbuf, nr);

  return 0;
}

static void pspsound_close(void)
{
	pspaudio_shutdown();
}

static int pspsound_buffill(void)
{
    int r;
    r = num_audio_buffs*audio_buff_size / 2;	// stops the re-inits, don't know why atm...
    
    if (r < 0)
      return 0;
    if (r > FRAGNR*FRAG)
      return FRAGNR*FRAG;
    return r;
}

static sound_device_t pspsound_device =
{
    "pspsound",
    pspsound_init,
    pspsound_write,
    NULL,
    NULL,
    NULL, /*pspsound_buffill,*/
    pspsound_close,
    NULL,
    NULL,
    1
};

int sound_init_psp_device(void)
{
    return sound_register_device(&pspsound_device);
}
