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
#include <malloc.h>
#include <string.h>
#include "bitmap.h"
#include "g2.h"

void render_bitmap (BITMAP *bitmap, int x, int y) {
  g2_put_image(x, y, bitmap->w, bitmap->h, bitmap->w, bitmap->h, bitmap->data);
}

int text_len (FONT *font, char *text) {
  int i, x = 0, l = strlen(text);
  for (i=0;i<l;i++) {
    if ((unsigned char)text[i] == 0xFD) {
      i+=3;
      continue;
    }
    if ((unsigned char)text[i] == 0xFC)
      continue;
    x += font->data[(unsigned char)text[i]-0x20].w;
  }
  return x;
}

void render_text_c (FONT *font, int xk, int yk, int w, const char *text, unsigned int color) {
  int l = text_len(font, text);
  render_text(font, xk+w/2-l/2, yk, text, color);
}

void render_text_r (FONT *font, int xk, int yk, const char *text, unsigned int color) {
  int l = text_len(font, text);
  render_text(font, xk-l, yk, text, color);
}

void render_text (FONT *font, int xk, int yk, const char *text, unsigned int color) {
  BITMAP2 foo;
  static u32 data[64*64] __attribute__((aligned(64)));
  u32 drawcolor = color;
  
  int x, i, j, l = strlen(text);
  x = 0;
  for (i=0;i<l;i++) {
    if ((unsigned char)text[i]==0xFD) {
      drawcolor = ((unsigned char)text[i+1]<<16) | ((unsigned char)text[i+2]<<8) | (unsigned char)text[i+3];
      i+=3;
      continue;
    } else if ((unsigned char)text[i]==0xFC) {
      drawcolor = color;
      continue;
    }
    foo.w = font->data[(unsigned char)text[i]-0x20].w;
    foo.h = font->h;
    foo.data=data;
    for (j=0;j<foo.w*foo.h;j++) {
      if (font->data[(unsigned char)text[i]-0x20].data[j])
        foo.data[j] = 0x7F000000|drawcolor;
      else
        foo.data[j] = 0x00000000;
    }
    render_bitmap ((BITMAP*)&foo, xk + x, yk);
    x += font->data[(unsigned char)text[i]-0x20].w;
  }
  
}
