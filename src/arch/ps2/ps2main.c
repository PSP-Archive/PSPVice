/*
 * ps2main.c - PS2 startup.
 *
 * Written by
 *  Rami Räsänen <raipsu@users.sourceforge.net>
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

#include <tamtypes.h>
#include <eefileio.h>
#include <malloc.h>
#include <kernel.h>
#include <compat.h>
#include <sifcmd.h>
#include <sifrpc.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

#include "log.h"
#include "machine.h"
#include "main.h"
#include "video.h"
#include "libpad.h"
#include "loadmodule.h"
#include "gs.h"
#include "g2.h"
#include "hw.h"
#include "sjpcm.h"
#include "support.h"
#include "ps2kbd.h"
#include "vkeyboard.h"
#include "cdvd_vsync.h"
#include "pad.h"

char **ps2_argv;
int ps2_argc;

char path_prefix[512];

void graph_init() {
  g2_init(PAL_C64);
/*
    disp_w = 440;
    disp_h = 290;
    curmode = T_PAL;
*/
  g2_set_visible_frame(1);
  g2_set_active_frame(0);

  g2_set_fill_color(0, 0, 0);
  g2_fill_rect(0, 0, g2_get_max_x(), g2_get_max_y());
  g2_flip_buffers();
  g2_fill_rect(0, 0, g2_get_max_x(), g2_get_max_y());
}

void pcm_init() {
  if (_sifLoadModule("rom0:LIBSD", 0, NULL, 0) < 0) {
    printf ("Failed to load rom0:LIBSD.\n");
  }
  loadmodule_robust("sjpcm.irx");
  if (SjPCM_Init(1) < 0) {
    printf ("SjPCM_Init() failed.\n");
  }
}

void kbd_init() {
  if (loadmodule_robust("npm-usbd.irx"))
    loadmodule_robust("usbd.irx");
  loadmodule_robust("ps2kbd.irx");
}

extern FILE *tempfile[4];

FILE *fopen_(const char *c, const char *d, int test) {
  FILE *new = malloc(sizeof(FILE));

  bzero((char*)new, sizeof(FILE));

  printf ("opening '%s' '%s'\n", c, d);

  if (!strncmp(c, "cdfs:", 5) || !strncmp(c, "cdrom0:", 7)) {
    cdvd_access();
  }

  new->_ur = -1;   /* temp count */

  if (!strncmp(c, "temp", 4)) {
    new->_ur = atoi(c+4);
  }

  if (new->_ur < -1 || new->_ur > 4) {
    free(new);
    return NULL;
  }

  new->_flags = 0;

  if (index(d, 'w')!=NULL || index(d, '+')!=NULL) {
    if (new->_ur==-1)
      new->_file = fio_open(c, O_RDWR|O_CREAT, 1);
    new->_flags |= 1;
    new->_r = 0;
    new->_offset = 0;
    new->_p = malloc(new->_r + 1);
    if (new->_ur!=-1) {
      if (tempfile[new->_ur]!=NULL) {
        free(tempfile[new->_ur]->_p);
        free(tempfile[new->_ur]);
      }
      tempfile[new->_ur]=new;
    }
    return new;
  } else {
    if (new->_ur!=-1) {
      free(new);
      if (tempfile[new->_ur]==NULL) {
        return NULL;
      }
      tempfile[new->_ur]->_offset = 0;
      tempfile[new->_ur]->_flags = 0;
      return tempfile[new->_ur];
    }
    new->_file = fio_open(c, O_RDONLY, 1);
  }

  if (new->_file<0) {
    free(new);
    return NULL;
  }
  if (test) {
    fio_close(new->_file);
    free(new);
    return (FILE*)1;
  }

  new->_r = fioLseek(new->_file, 0, SEEK_END);
  new->_p = malloc(new->_r + 1);
  fioLseek(new->_file, 0, SEEK_SET);
  new->_offset = 0;

  if (new->_r < 0 || new->_p == NULL) {
    fio_close(new->_file);
    if (new->_p != NULL)
      free(new->_p);
    free(new);
    return NULL;
  }

  if (new->_r) {
    if (fio_read(new->_file, new->_p, new->_r)!=new->_r) {
      fio_close(new->_file);
      free(new->_p);
      free(new);
      return NULL;
    }
  }

  if (!(new->_flags & 1))
    fio_close(new->_file);

  return new;
}

int main(int argc, char **argv)
{

  ps2_argv = argv; ps2_argc = argc;

  if (argc>=1) {
    char *p;

    if ((p = rindex(argv[0], '/'))!=NULL) {
      snprintf(path_prefix, sizeof(path_prefix), "%s", argv[0]);
      p = rindex(path_prefix, '/');
      if (p!=NULL)
        p[1]='\0';
    } else if ((p = rindex(argv[0], '\\'))!=NULL) {
      snprintf(path_prefix, sizeof(path_prefix), "%s", argv[0]);
      p = rindex(path_prefix, '\\');
      if (p!=NULL)
        p[1]='\0';
    } else if ((p = index(argv[0], ':'))!=NULL) {
      snprintf(path_prefix, sizeof(path_prefix), "%s", argv[0]);
      p = index(path_prefix, ':');
      if (p!=NULL)
        p[1]='\0';
    }
  }

//  strcpy(path_prefix, "cdrom0:\\");

  SifInitRpc(0);

  graph_init();

  pad_init();

  _sifLoadModule("rom0:SIO2MAN", 0, NULL, 0);
  _sifLoadModule("rom0:CDVDMAN", 0, NULL, 0);
  _sifLoadModule("rom0:MCMAN", 0, NULL, 0);
  _sifLoadModule("rom0:MCSERV", 0, NULL, 0);

  pcm_init();

  kbd_init();

  vkeyboard_init();
  ps2_keyboard_init();

  if (!strncmp(path_prefix, "cdrom0:", 6)) {
    cdvd_access();
  }

  if (!strncmp(path_prefix, "cdrom0:", 7)) {
    char buf[512];
    snprintf (buf, sizeof(buf), "cdfs:/%s", path_prefix+8);
    strcpy(path_prefix, buf);
  }

  return main_program(argc, argv);
}

void main_exit(void)
{
    log_message(LOG_DEFAULT, "\nExiting...");

    machine_shutdown();

    putchar ('\n');
}

