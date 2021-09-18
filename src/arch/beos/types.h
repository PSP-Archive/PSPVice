/*
 * types.h - Type definitions for VICE.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andr� Fachat <a.fachat@physik.tu-chemnitz.de>
 *  Teemu Rantanen <tvr@cs.hut.fi>
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

#ifndef _TYPES_H
#define _TYPES_H

#include "vice.h"

#define BYTE unsigned char

typedef signed char SIGNED_CHAR;

#if SIZEOF_UNSIGNED_SHORT == 2
typedef unsigned short WORD;
typedef signed short SWORD;
#else
#error Cannot find a proper 16-bit type!
#endif

#if SIZEOF_UNSIGNED_INT == 4
typedef unsigned int DWORD;
typedef signed int SDWORD;
#elif SIZEOF_UNSIGNED_LONG == 4
typedef unsigned long DWORD;
typedef signed long SWORD;
#else
#error Cannot find a proper 32-bit type!
#endif

typedef DWORD CLOCK;
/* Maximum value of a CLOCK.  */
#define CLOCK_MAX (~((CLOCK)0))

/* problems with REGPARM */
#define NO_REGPARM 1

#if defined(__GNUC__) && !defined(NO_REGPARM)
#define REGPARM1 __attribute__((regparm(1)))
#define REGPARM2 __attribute__((regparm(2)))
#define REGPARM3 __attribute__((regparm(3)))
#else
#define REGPARM1
#define REGPARM2
#define REGPARM3
#endif

#endif  /* _TYPES_H */

