#ifndef _TYPES_H
#define _TYPES_H

#include "vice.h"
#ifdef WIN32
#include "Win32PSPSpecific.h"
#else
#include "PSPSpecific.h"
#endif

//typedef unsigned int u32;
//typedef unsigned char u8;

#define BYTE unsigned char
#define NO_REGPARM

typedef signed char SIGNED_CHAR;

typedef unsigned short WORD;
typedef signed short SWORD;

typedef unsigned int DWORD;
typedef signed int SDWORD;

typedef DWORD CLOCK;
/* Maximum value of a CLOCK.  */
#define CLOCK_MAX (~((CLOCK)0))

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

