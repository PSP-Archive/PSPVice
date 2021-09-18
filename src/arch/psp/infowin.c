#include <stdio.h>
#include "types.h"
#include <string.h>
#include "pad.h"
#include "bitmap.h"

#define disp_w 480
#define disp_h 272

#define WINX (disp_w/2-im_winw/2)
#define WINY (disp_h/2-WINH/2)
#define WINH 21 // Height of info frame

int infowin_active = 0;
int infomsg_active = 0;
int aboutwin_active = 0;

static char iwmessage[512];
static char immessage[512];
static int l, l2, iwinw;
static int im_winw, im_l;
static int im_timeout;

// -------------------------------------------------------------
// Name : 
// -------------------------------------------------------------
void show_info_msg(char *message, int timeout)
{
  infomsg_active = 1;
  im_timeout = timeout;
  snprintf(immessage, sizeof(immessage), "%s", message);
  im_l = text_len(NULL, immessage);
  im_winw = im_l + 10;
}

// -------------------------------------------------------------
// Name : 
// -------------------------------------------------------------
void infomsg_frame()
{
  if (infomsg_active)
  {
    PSPSetFillColor(20, 20, 40, 192);
    PSPFillRectangle(WINX, WINY, WINX+im_winw, WINY+WINH);
    render_text_c (NULL, WINX, WINY+((WINH-8)/2), im_winw, immessage, 0xF0F0F0);
    if (--im_timeout < 1)
      infomsg_active = 0;
  }  
}

#undef WINX
#undef WINH
#define WINX (disp_w/2-iwinw/2)
#define WINH 50

// -------------------------------------------------------------
// Name : 
// -------------------------------------------------------------
void show_infowin_nonblock(char *message)
{
  infowin_active = 1;
  snprintf(iwmessage, sizeof(iwmessage), "%s", message);
  l = text_len(NULL, "Press X");
  l2 = text_len(NULL, iwmessage);
  if (l2 > l)
    l = l2;
  iwinw = l + 10;
}

// -------------------------------------------------------------
// Name : 
// -------------------------------------------------------------
void infowin_frame()
{
  PSPSetFillColor(20, 20, 40,192);
  PSPFillRectangle(WINX, WINY, WINX+iwinw, WINY+WINH);
  render_text_c (NULL, WINX, WINY-2, iwinw, iwmessage, 0xF0F0F0);
  render_text_c (NULL, WINX, WINY+WINH-21, iwinw, "Press X", 0x90F090);

  if (new_pad & PAD_CROSS)
    infowin_active = 0;
}

#undef WINW
#undef WINH
#undef WINX
#define WINW 400
#define WINH (aboutwin_h)
#define WINX (disp_w/2-WINW/2)

extern int is_paused;

const char about_text[] = "\nVICE 1.22\n\n\
Versatile Commodore Emulator\n\n\
Copyright (c) 1998-2007 Andreas Boose\n\
Copyright (c) 1998-2007 Tibor Biczo\n\
Copyright (c) 1999-2007 Andreas Dehmel\n\
Copyright (c) 1999-2007 Andreas Matthies\n\
Copyright (c) 1999-2007 Martin Pottendorfer\n\
Copyright (c) 1998-2007 Dag Lem\n\
Copyright (c) 2000-2007 Spiro Trikaliotis\n\
Copyright (c) 2005-2007 Marco van den Heuvel\n\
Copyright (c) 2006-2007 Christian Vogelgsang\n\
Copyright (c) 1999-2005 Thomas Bretz\n\
Copyright (c) 2003-2005 David Hansel\n\
Copyright (c) 2000-2004 Markus Brenner\n\
Official VICE homepage:http://www.viceteam.org/\n\
PS2 version Copyright (c) 2004 Rami Rasanen\n\
PSP version 2005-2007 Christophe Kohler";

static const char *aboutwin_text = about_text;
static int aboutwin_h = 196;
static int do_pause = 1;

// -------------------------------------------------------------
// Name : 
// -------------------------------------------------------------
void show_aboutwin_nonblock()
{
  aboutwin_text = about_text;
  aboutwin_h = 196;
  aboutwin_active = 1;
  do_pause = 1;
}

// -------------------------------------------------------------
// Name : 
// -------------------------------------------------------------
void aboutwin_set_text(const char *text, int h)
{
  aboutwin_text = text;
  aboutwin_h = h;
  aboutwin_active = 1;
  do_pause = 0;
}

// -------------------------------------------------------------
// Name : 
// -------------------------------------------------------------
void aboutwin_frame()
{

  int y = 5;
  const char *text = aboutwin_text;

  int pos = 0;
  char buf[256];
  static int wasnotpaused = 0;

  if (do_pause)
  {
    if (!is_paused) {
      wasnotpaused = 1;
      is_paused = 1;
    }
  }

  PSPSetFillColor(20, 20, 40,192);
  PSPFillRectangle(WINX, WINY, WINX+WINW, WINY+WINH);

  text--;

  do {
    buf[pos++] = *++text;
    if (buf[pos-1] == '\n' || buf[pos-1] == '\0') {
      buf[pos-1] = '\0';
      render_text_c (NULL, WINX, WINY+y, WINW, buf, 0xF0F0F0);
      if (strlen(buf)==0)
        y+=5;
      else
        y+=9;
      pos = 0;
    }
  } while (*text != '\0');

  render_text_c (NULL, WINX, WINY+WINH-16, WINW, "Press X", 0x90F090);

  if (new_pad & (PAD_CROSS | PAD_TRIANGLE)) {
    aboutwin_active = 0;
    if (do_pause) {
      if (wasnotpaused)
        is_paused = 0;
      wasnotpaused = 0;
    }
  }

}
