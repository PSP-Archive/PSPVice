/*
 * uicmdline.c - The command line user interface.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Thomas Bretz <tbretz@gsi.de>
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

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "cmdline.h"
#include "lib.h"
#include "translate.h"
#include "uilib.h"


/* This does not work as stdout is directed to null.
   This function is temporarily extended during the
   transition to string tables.  */

void ui_cmdline_show_help(unsigned int num_options,
                          cmdline_option_ram_t *options,
                          void *userparam)
{
    unsigned int i;

    printf("\nAvailable command-line options:\n\n");
    for (i = 0; i < num_options; i++) {
        fputs(options[i].name, stdout);
        if (options[i].need_arg && options[i].param_name != 0)
            printf(" %s", translate_text(options[i].param_name));
        printf("\n\t%s\n", translate_text(options[i].description));
    }
    putchar('\n');
}
