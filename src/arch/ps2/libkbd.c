/*
 * libkbd.c - USB Keyboard Driver for PS2
 *
 * (c) 2003 TyRaNiD <tiraniddo@hotmail.com>
 *
 * This library provides access functions for the keyboard driver
 *
 * See the file LICENSE included with this distribution for licensing terms.
 */

#include <tamtypes.h>
#include <eefileio.h>
#include "libkbd.h"

static int kbd_fd = -1;
static int curr_blockmode = PS2KBD_NONBLOCKING;
static int curr_readmode = PS2KBD_READMODE_NORMAL;

int PS2KbdInit(void)
/* Initialise the keyboard library */
{
  if(kbd_fd >= 0) /* Already initialised */
    {
      return 0;
    }

  kbd_fd = fioOpen(PS2KBD_DEVFILE, 0, 1);
  if(kbd_fd < 0)
    {
      return 0;
    }

  return 1;
}

int PS2KbdRead(char *key)
/* Reads 1 character from the keyboard */
{
  if((kbd_fd >= 0) && (curr_readmode == PS2KBD_READMODE_NORMAL))
    {
      return fioRead(kbd_fd, key, 1);
    }

  return 0;
}

int PS2KbdReadRaw(PS2KbdRawKey *key)
/* Reads 1 raw character from the keyboard */
{
  if((kbd_fd >= 0) && (curr_readmode == PS2KBD_READMODE_RAW))
    {
      return fioRead(kbd_fd, key, 2) / 2;
    }

  return 0;
}

int PS2KbdSetReadmode(u32 readmode)
/* Sets the read mode to normal or raw */
{
  if((kbd_fd >= 0) && (curr_readmode != readmode))
    {
      curr_readmode = readmode;
      return fioIoctl(kbd_fd, PS2KBD_IOCTL_SETREADMODE, &readmode);
    }
  return 0;
}

int PS2KbdSetBlockingMode(u32 blockmode)
/* Sets the blocking mode on or off */
{
  if((kbd_fd >= 0) && (curr_blockmode != blockmode))
    {
      return fioIoctl(kbd_fd, PS2KBD_IOCTL_SETBLOCKMODE, &blockmode);
    }

  return 0;
}

int PS2KbdSetRepeatRate(u32 repeat)
/* Sets the repeat rate in millseconds */
{
  if(kbd_fd >= 0)
    {
      return fioIoctl(kbd_fd, PS2KBD_IOCTL_SETREPEATRATE, &repeat);
    }
  return 0;
}

int PS2KbdSetLeds(u8 leds)
/* Sets all connected keyboards leds */
{
  if(kbd_fd >= 0)
    {
      return fioIoctl(kbd_fd, PS2KBD_IOCTL_SETLEDS, &leds);
    }
  return 0;
}

int PS2KbdSetKeymap(PS2KbdKeyMap *keymaps)
/* Sets the current keymap */
{
  if(kbd_fd >= 0)
    {
      return fioIoctl(kbd_fd, PS2KBD_IOCTL_SETKEYMAP, keymaps);
    }
  return 0;
}

int PS2KbdSetCtrlmap(u8 *ctrlmap)
/* Sets the control key mappings */
{
  if(kbd_fd >= 0)
    {
      return fioIoctl(kbd_fd, PS2KBD_IOCTL_SETCTRLMAP, ctrlmap);
    }
  return 0;
}

int PS2KbdSetAltmap(u8 *altmap)
/* Sets the alt key mappings */
{
  if(kbd_fd >= 0)
    {
      return fioIoctl(kbd_fd, PS2KBD_IOCTL_SETALTMAP, altmap);
    }
  return 0;
}

int PS2KbdSetSpecialmap(u8 *special)
/* Sets the special key mappings */
{
  if(kbd_fd >= 0)
    {
      return fioIoctl(kbd_fd, PS2KBD_IOCTL_SETSPECIALMAP, special);
    }
  return 0;
}

int PS2KbdFlushBuffer(void)
/* Flushes the keyboard buffer */
{
  int dummy;

  if(kbd_fd >= 0)
    {
      return fioIoctl(kbd_fd, PS2KBD_IOCTL_FLUSHBUFFER, &dummy);
    }
  return 0;
}

int PS2KbdResetKeymap(void)
/* Resets the keymap to the default US mapping */
{
  int dummy;

  if(kbd_fd >= 0)
    {
      return fioIoctl(kbd_fd, PS2KBD_IOCTL_RESETKEYMAP, &dummy);
    }
  return 0;
}

int PS2KbdClose(void)
/* Close down the keyboard library */
{
  if(kbd_fd >= 0)
    {
      fioClose(kbd_fd);
      kbd_fd = -1;
    }

  return 1;
}
