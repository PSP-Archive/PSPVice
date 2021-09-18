/*
  Written by Rami Räsänen <raipsu@users.sourceforge.net>

  This file is part of PS2VIC.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
  02111-1307  USA.
*/

#include <tamtypes.h>
#include <kernel.h>
#include <compat.h>
#include <eefileio.h>
#include <sifcmd.h>
#include <sifrpc.h>
#include <stdio.h>
#include <string.h>
#include "osdmsg.h"
#include "loadmodule.h"
#include "support.h"
#include "ps2main.h"

int loadmodule_robust(char *name) {
  char fn[128];
  char upper[128];
  int i = 0;

  snprintf (fn, sizeof(fn), "%s%s", path_prefix, name);
  show_msg(fn);

  if (_sifLoadModule(fn, 0, NULL, 0) < 0) {
    snprintf (upper, sizeof(upper), "%s", name);
    while (upper[i]!='\0') {
      upper[i] = mtoupper(upper[i]);
      i++;
    }
    snprintf (fn, sizeof(fn), "%s%s", path_prefix, upper);
    show_msg(fn);
    if (_sifLoadModule(fn, 0, NULL, 0) < 0) {
      snprintf (fn, sizeof(fn), "%s%s;1", path_prefix, upper);
      show_msg(fn);
      if (_sifLoadModule(fn, 0, NULL, 0) < 0) {
        return 1;
      }
    }
  }
  return 0;
}

int fio_open_robust(char *name, int mode) {
  int i=0;
  int f;
  char fn[256];
  char upper[256];

  snprintf (upper, sizeof(upper), "%s", name);
  while (upper[i]!='\0') {
    upper[i] = mtoupper(upper[i]);
    i++;
  }

  snprintf (fn, sizeof(fn), "%s", name);
  if ((f=fio_open(fn, mode, 1))<0) {
    snprintf (fn, sizeof(fn), "%s", upper);
    if ((f=fio_open(fn, mode, 1))<0) {
      snprintf (fn, sizeof(fn), "%s;1", upper);
      if ((f=fio_open(fn, mode, 1))<0) {
        snprintf (fn, sizeof(fn), "%s%s", path_prefix, name);
        if ((f=fio_open(fn, mode, 1))<0) {
          snprintf (fn, sizeof(fn), "%s%s", path_prefix, upper);
          if ((f=fio_open(fn, mode, 1))<0) {
            snprintf (fn, sizeof(fn), "%s%s;1", path_prefix, upper);
            if ((f=fio_open(fn, mode, 1))<0)
              return -1;
          }
        }
      }
    }
  }

  return f;
}
