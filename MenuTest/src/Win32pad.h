#ifndef _PAD_H
#define _PAD_H

void pad_update();
void pad_init();

extern unsigned int new_pad;
extern unsigned int old_pad;
extern unsigned int new_rel;
extern unsigned int paddata;

struct padButtonStatus
{
  unsigned char ljoy_h;
  unsigned char ljoy_v;
};

extern struct padButtonStatus buttons;


enum PspCtrlButtons
{
	/** Select button. */
	PSP_CTRL_SELECT     = 0x000001,
	/** Start button. */
	PSP_CTRL_START      = 0x000008,
	/** Up D-Pad button. */
	PSP_CTRL_UP         = 0x000010,
	/** Right D-Pad button. */
	PSP_CTRL_RIGHT      = 0x000020,
	/** Down D-Pad button. */
	PSP_CTRL_DOWN      	= 0x000040,
	/** Left D-Pad button. */
	PSP_CTRL_LEFT      	= 0x000080,
	/** Left trigger. */
	PSP_CTRL_LTRIGGER   = 0x000100,
	/** Right trigger. */
	PSP_CTRL_RTRIGGER   = 0x000200,
	/** Triangle button. */
	PSP_CTRL_TRIANGLE   = 0x001000,
	/** Circle button. */
	PSP_CTRL_CIRCLE     = 0x002000,
	/** Cross button. */
	PSP_CTRL_CROSS      = 0x004000,
	/** Square button. */
	PSP_CTRL_SQUARE     = 0x008000,
	/** Home button. */
	PSP_CTRL_HOME       = 0x010000,
	/** Hold button. */
	PSP_CTRL_HOLD       = 0x020000,
	/** Music Note button. */
	PSP_CTRL_NOTE       = 0x800000,
}; 

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

#endif
