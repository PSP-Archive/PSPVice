#include <stdio.h>
#include "types.h"
#include <malloc.h>
#include "pad.h"

unsigned int paddata = 0; // Pressed
unsigned int new_pad = 0; // Just pressed
unsigned int new_rel = 0; // Just released
unsigned int old_pad = 0; // Save previous state to compute the 2 above values

struct padButtonStatus buttons;

void pad_loadmodules()
{
}

void pad_init()
{
  paddata = 0;
  new_pad = 0;
  new_rel = 0;
  old_pad = 0;
}


void pad_update()
{
	SceCtrlData		buf;  

	sceCtrlReadBufferPositive(&buf, 1);
	
	paddata=buf.Buttons;;
  
  new_pad = paddata & ~old_pad;
  new_rel = ~paddata & old_pad;
  old_pad = paddata;

  buttons.ljoy_h = buf.Lx;
  buttons.ljoy_v = buf.Ly;
}
