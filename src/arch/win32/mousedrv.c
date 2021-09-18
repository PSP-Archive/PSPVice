/*
 * mousedrv.c - Mouse handling for Win32
 *
 * Written by
 *  Tibor Biczo <crown@mail.matav.hu>
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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "vice.h"

#include <windows.h>
#include <dinput.h>
#include <stdio.h>

#include "log.h"
#include "mouse.h"
#include "mousedrv.h"
#include "types.h"
#include "ui.h"


int _mouse_x, _mouse_y;
static int mouse_acquired = 0;
LPDIRECTINPUTDEVICE di_mouse = NULL;

#ifndef HAVE_GUIDLIB
const GUID GUID_XAxis = { 0xA36D02E0, 0xC9F3, 0x11CF,
                        { 0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 } };
const GUID GUID_YAxis = { 0xA36D02E1, 0xC9F3, 0x11CF,
                        { 0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 } };
const GUID GUID_Button = { 0xA36D02F0, 0xC9F3, 0x11CF,
                         { 0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 } };
const GUID GUID_SysMouse = { 0x6F1D2B60, 0xD5A0, 0x11CF,
                           { 0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00} };
#endif

typedef struct mouse_data_t {
    DWORD X;
    DWORD Y;
    BYTE LeftButton;
    BYTE RightButton;
    BYTE padding[2];
} mouse_data;

DIOBJECTDATAFORMAT mouse_objects[] = {
    { &GUID_XAxis, 0, DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_YAxis, 4, DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
    { &GUID_Button, 8, DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
    { &GUID_Button, 9, DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 }
};

DIDATAFORMAT mouse_data_format={
    sizeof(DIDATAFORMAT),
    sizeof(DIOBJECTDATAFORMAT),
    DIDF_RELAXIS,
    sizeof(mouse_data),
    4,
    mouse_objects
};

/* ------------------------------------------------------------------------- */

void mousedrv_mouse_changed(void)
{
    mouse_update_mouse_acquire();
}

int mousedrv_resources_init(void)
{
    return 0;
}

int mousedrv_cmdline_options_init(void)
{
    return 0;
}

/* ------------------------------------------------------------------------- */

void mouse_set_format(void)
{
    HRESULT result;

    result = IDirectInputDevice_SetDataFormat(di_mouse, &mouse_data_format);
    if (result != DI_OK) {
        log_debug("Can't set Mouse DataFormat");
        di_mouse = NULL;
    }
}

void mouse_update_mouse(void)
{
    mouse_data state;
    HRESULT result;

    if (di_mouse == NULL)
        return;

    result = DIERR_INPUTLOST;
    while (result == DIERR_INPUTLOST) {
        result = IDirectInputDevice_GetDeviceState(di_mouse, sizeof(mouse_data),
                                                   &state);
        if (result == DIERR_INPUTLOST) {
            result = IDirectInputDevice_Acquire(di_mouse);
            if (result != DI_OK) {
                return;
            }
        }
    }
    if (result != DI_OK) return;

    _mouse_x += state.X;
    _mouse_y += state.Y;

    mouse_button_left((int)(state.LeftButton & 0x80));
    mouse_button_right((int)(state.RightButton & 0x80));
}

void mousedrv_init(void)
{
}

void mouse_set_cooperative_level(void)
{
    HRESULT result;

    result = IDirectInputDevice_SetCooperativeLevel(
             di_mouse, ui_active_window, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
    if (result != DI_OK) {
        log_debug("Warning: couldn't set cooperation level of mice to exclusive! %i",
                  (int)ui_active_window);
        di_mouse = NULL;
    }
}

void mouse_update_mouse_acquire(void)
{
    if (di_mouse == NULL)
        return;
    if (_mouse_enabled) {
        if (ui_active) {
            mouse_set_cooperative_level();
            IDirectInputDevice_Acquire(di_mouse);
            mouse_acquired = 1;
        } else {
            IDirectInputDevice_Unacquire(di_mouse);
            mouse_acquired = 0;
        }
    } else {
        if (mouse_acquired) {
            IDirectInputDevice_Unacquire(di_mouse);
            mouse_acquired = 0;
        }
    }
}

BYTE mousedrv_get_x(void)
{
    if (!_mouse_enabled)
        return 0xff;
    return (BYTE)(_mouse_x >> 1) & 0x7e;
}

BYTE mousedrv_get_y(void)
{
    if (!_mouse_enabled)
        return 0xff;
    return (BYTE)(~_mouse_y >> 1) & 0x7e;
}

