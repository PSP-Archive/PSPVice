/*
 * joy.c - Joystick support for MS-DOS.
 *
 * Written by
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

#include "vice.h"

#include <stdio.h>
#include "types.h"

#include "cmdline.h"
#include "keyboard.h"
#include "joy.h"
#include "joystick.h"
#include "resources.h"
#include "pad.h"
#include "PSPInputs.h"

#include "vkeyboard.h"

int joystick_port_map[2];

int swap_joysticks = 1; // CK ... start with joystick swapped

static int joyport1select(resource_value_t v, void *param)
{
    joystick_port_map[0] = (int)v;
    return 0;
}

static int joyport2select(resource_value_t v, void *param)
{
    joystick_port_map[1] = (int)v;
    return 0;
}

/*
static const resource_int_s resources[] = {
    { "JoyDevice1", RES_INTEGER, (resource_value_t)0,
      (void *)&joystick_port_map[0], joyport1select, NULL },
    { "JoyDevice2", RES_INTEGER, (resource_value_t)0,
      (void *)&joystick_port_map[1], joyport2select, NULL },
    { NULL }
};
*/

static const resource_int_t resources[] = {
    { "JoyDevice1", JOYDEV_NONE, RES_EVENT_NO, NULL,
      &joystick_port_map[0], joyport1select, NULL },
    { "JoyDevice2", JOYDEV_NONE, RES_EVENT_NO, NULL,
      &joystick_port_map[1], joyport1select, NULL },
    { NULL }
};
 

int joystick_arch_init_resources(void)
{
    return resources_register_int(resources);
}

int joystick_init_cmdline_options(void)
{
  return 0;
}

static int joystick_init_done = 0;

int joy_arch_init(void)
{
    if (joystick_init_done)
        return 0;

    joystick_init_done = 1;
    return 0;
}

void joy_setabs(void)
{
  int value = 0;

  if ( vkeyboard_IsActive() )
  	return;

  if ( PSPInputs_IsButtonPressed(pspinp_J1_LEFT) )
    value |= 4;
  if ( PSPInputs_IsButtonPressed(pspinp_J1_RIGHT) )
    value |= 8;
  if ( PSPInputs_IsButtonPressed(pspinp_J1_UP) )
    value |= 1;
  if ( PSPInputs_IsButtonPressed(pspinp_J1_DOWN) )
    value |= 2;
  if ( PSPInputs_IsButtonPressed(pspinp_J1_FIRE) )
    value |= 16;
  
  #define KEYBH_MIN (128.f-100.0f)
  #define KEYBH_MAX (128.f+100.0f)

  #define KEYBV_MIN (128.f-100.0f)
  #define KEYBV_MAX (128.f+100.0f)

  // -- 
  if ( ! vkeyboard_IsActive() )
  {
	  if (buttons.ljoy_h<KEYBH_MIN)
	    value |= 4;
	  if (buttons.ljoy_h>KEYBH_MAX)
	    value |= 8;
	  if (buttons.ljoy_v<KEYBV_MIN)
	    value |= 1;
	  if (buttons.ljoy_v>KEYBV_MAX)
	    value |= 2;
  }
  joystick_set_value_absolute(swap_joysticks?2:1, value);
}

void joy2_setabs(void)
{
  if ( vkeyboard_IsActive() )
  	return;

  int value = 0;

  if ( PSPInputs_IsButtonPressed(pspinp_J2_LEFT) )
    value |= 4;
  if ( PSPInputs_IsButtonPressed(pspinp_J2_RIGHT) )
    value |= 8;
  if ( PSPInputs_IsButtonPressed(pspinp_J2_UP) )
    value |= 1;
  if ( PSPInputs_IsButtonPressed(pspinp_J2_DOWN) )
    value |= 2;
  if ( PSPInputs_IsButtonPressed(pspinp_J2_FIRE) )
    value |= 16;
  
  joystick_set_value_absolute(swap_joysticks?1:2, value);
}

void joystick_update(void)
{
}

void joystick_close(void)
{
   return;
}


