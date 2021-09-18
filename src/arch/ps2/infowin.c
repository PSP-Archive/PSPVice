#include <stdio.h>
#include <tamtypes.h>
#include <string.h>
#include "gs.h"
#include "g2.h"
#include "libpad.h"
#include "pad.h"
#include "bitmap.h"

#define WINX (disp_w/2-im_winw/2)
#define WINY (disp_h/2-WINH/2)
#define WINH 21

int infowin_active = 0;
int infomsg_active = 0;
int aboutwin_active = 0;

static char iwmessage[512];
static char immessage[512];
static int l, l2, iwinw;
static int im_winw, im_l;
static int im_timeout;

extern FONT f_arial;
extern FONT f_arial7;

void show_info_msg(char *message, int timeout) {
  infomsg_active = 1;
  im_timeout = timeout;
  snprintf(immessage, sizeof(immessage), "%s", message);
  im_l = text_len(&f_arial, immessage);
  im_winw = im_l + 10;
}

void infomsg_frame() {
  if (infomsg_active) {
    g2_set_fill_color(20, 20, 40);
    g2_fill_rect(WINX, WINY, WINX+im_winw, WINY+WINH);
    g2_set_fill_color(0, 0, 0);
    render_text_c (&f_arial, WINX, WINY-2, im_winw, immessage, 0xF0F0F0);
    g2_set_fill_color(0, 0, 0);
    if (--im_timeout < 1)
      infomsg_active = 0;
  }  
}

#undef WINX
#undef WINH
#define WINX (disp_w/2-iwinw/2)
#define WINH 50

void show_infowin_nonblock(char *message) {
  infowin_active = 1;
  snprintf(iwmessage, sizeof(iwmessage), "%s", message);
  l = text_len(&f_arial, "Press X");
  l2 = text_len(&f_arial, iwmessage);
  if (l2 > l)
    l = l2;
  iwinw = l + 10;
}

void infowin_frame() {
  g2_set_fill_color(20, 20, 40);
  g2_fill_rect(WINX, WINY, WINX+iwinw, WINY+WINH);
  g2_set_fill_color(0, 0, 0);
  render_text_c (&f_arial, WINX, WINY-2, iwinw, iwmessage, 0xF0F0F0);
  render_text_c (&f_arial, WINX, WINY+WINH-21, iwinw, "Press X", 0x90F090);
  g2_set_fill_color(0, 0, 0);

  if (new_pad & PAD_CROSS)
    infowin_active = 0;
}


#undef WINW
#undef WINH
#undef WINX
#define WINW 280
#define WINH (aboutwin_h)
#define WINX (disp_w/2-WINW/2)

extern int is_paused;

const char about_text[] = "VICE 1.14\n\n\
Versatile Commodore Emulator\n\n\
Copyright (c) 1998-2004 Andreas Boose\n\
Copyright (c) 1998-2004 Dag Lem\n\
Copyright (c) 1998-2004 Tibor Biczo\n\
Copyright (c) 1999-2004 Andreas Dehmel\n\
Copyright (c) 1999-2004 Thomas Bretz\n\
Copyright (c) 1999-2004 Andreas Matthies\n\
Copyright (c) 1999-2004 Martin Pottendorfer\n\
Copyright (c) 2000-2004 Markus Brenner\n\
Copyright (c) 2000-2004 Spiro Trikaliotis\n\
Copyright (c) 2003-2004 David Hansel\n\n\
Playstation 2 version\n\n\
Copyright (c) 2004 Rami Räsänen";

static const char *aboutwin_text = about_text;
static int aboutwin_h = 180;
static int do_pause = 1;

void show_aboutwin_nonblock() {
  aboutwin_text = about_text;
  aboutwin_h = 180;
  aboutwin_active = 1;
  do_pause = 1;
}

void aboutwin_set_text(const char *text, int h) {
  aboutwin_text = text;
  aboutwin_h = h;
  aboutwin_active = 1;
  do_pause = 0;
}

void aboutwin_frame() {
  int y = 0;
  const char *text = aboutwin_text;

  int pos = 0;
  char buf[256];
  static int wasnotpaused = 0;

  if (do_pause) {
    if (!is_paused) {
      wasnotpaused = 1;
      is_paused = 1;
    }
  }

  g2_set_fill_color(20, 20, 40);
  g2_fill_rect(WINX, WINY, WINX+WINW, WINY+WINH);
  g2_set_fill_color(0, 0, 0);

  text--;

  do {
    buf[pos++] = *++text;
    if (buf[pos-1] == '\n' || buf[pos-1] == '\0') {
      buf[pos-1] = '\0';
      render_text_c (&f_arial7, WINX, WINY+y, WINW, buf, 0xF0F0F0);
      if (strlen(buf)==0)
        y+=5;
      else
        y+=9;
      pos = 0;
    }
  } while (*text != '\0');

  render_text_c (&f_arial7, WINX, WINY+WINH-21, WINW, "Press X", 0x90F090);
  g2_set_fill_color(0, 0, 0);

  if (new_pad & (PAD_CROSS | PAD_TRIANGLE)) {
    aboutwin_active = 0;
    if (do_pause) {
      if (wasnotpaused)
        is_paused = 0;
      wasnotpaused = 0;
    }
  }
}
