#ifndef _COMMUNHEADERH
#define _COMMUNHEADERH

// Configuration of VICE. Options
#include "config.h"

#include <pspctrl.h> 

#define PAD_START    PSP_CTRL_START
#define PAD_SELECT   PSP_CTRL_SELECT
#define PAD_L2       0
#define PAD_L1       PSP_CTRL_LTRIGGER
#define PAD_R2       0
#define PAD_R1       PSP_CTRL_RTRIGGER
#define PAD_TRIANGLE PSP_CTRL_TRIANGLE
#define PAD_CROSS    PSP_CTRL_CROSS
#define PAD_SQUARE   PSP_CTRL_SQUARE
#define PAD_CIRCLE   PSP_CTRL_CIRCLE
#define PAD_UP       PSP_CTRL_UP
#define PAD_DOWN     PSP_CTRL_DOWN
#define PAD_LEFT     PSP_CTRL_LEFT
#define PAD_RIGHT    PSP_CTRL_RIGHT

#define printf	MyPrintf

#define BYTE unsigned char

#define feof fendoffile // for PSP (do not work)

#endif
