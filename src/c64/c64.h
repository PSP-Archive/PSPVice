/*
 * c64.h
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

#ifndef _C64_H
#define _C64_H

#define C64_PAL_CYCLES_PER_SEC  985248
#define C64_PAL_CYCLES_PER_LINE 63
#define C64_PAL_SCREEN_LINES    312
#define C64_PAL_CYCLES_PER_RFSH (C64_PAL_SCREEN_LINES \
                                 * C64_PAL_CYCLES_PER_LINE)
/* PAL refresh rate: 50.123432124542124 */
#define C64_PAL_RFSH_PER_SEC    (1.0 / ((double)C64_PAL_CYCLES_PER_RFSH \
                                        / (double)C64_PAL_CYCLES_PER_SEC))

#define C64_NTSC_CYCLES_PER_SEC  1022730
#define C64_NTSC_CYCLES_PER_LINE 65
#define C64_NTSC_SCREEN_LINES    263
#define C64_NTSC_CYCLES_PER_RFSH (C64_NTSC_SCREEN_LINES \
                                  * C64_NTSC_CYCLES_PER_LINE)
#define C64_NTSC_RFSH_PER_SEC    (1.0 / ((double)C64_NTSC_CYCLES_PER_RFSH \
                                        / (double)C64_NTSC_CYCLES_PER_SEC))

#define C64_NTSCOLD_CYCLES_PER_SEC  1022730
#define C64_NTSCOLD_CYCLES_PER_LINE 64
#define C64_NTSCOLD_SCREEN_LINES    262
#define C64_NTSCOLD_CYCLES_PER_RFSH (C64_NTSCOLD_SCREEN_LINES \
                                    * C64_NTSCOLD_CYCLES_PER_LINE)
#define C64_NTSCOLD_RFSH_PER_SEC  (1.0 / ((double)C64_NTSCOLD_CYCLES_PER_RFSH \
                                        / (double)C64_NTSCOLD_CYCLES_PER_SEC))

struct cia_context_s;
struct printer_context_s;
struct tpi_context_s;

typedef struct machine_context_s {
    struct cia_context_s *cia1;
    struct cia_context_s *cia2;
    struct tpi_context_s *tpi1;
    struct printer_context_s *printer[3];
} machine_context_t;

extern machine_context_t machine_context;

#endif

