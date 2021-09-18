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

#include <malloc.h>
#include <string.h>
#include "bitmap.h"
#include "PSPSpecific.h"

// -------------------------------------------------------------
//
// -------------------------------------------------------------
void render_bitmap (BITMAP *bitmap, int x, int y)
{
  PSPDisplayImage(x,y, bitmap);
}

// -------------------------------------------------------------
// --
// -------------------------------------------------------------
int text_len (void* unused, char *text)
{
 return strlen(text)*8;
}

// -------------------------------------------------------------
// -- Render text center
// -------------------------------------------------------------
void render_text_c (void* unused, int xk, int yk, int w, const char *text, unsigned int color)
{
  int l = text_len(NULL, (char*) text);
  render_text(NULL, xk+w/2-l/2, yk, text, color);
}

// -------------------------------------------------------------
// -- Render text right
// -------------------------------------------------------------
void render_text_r (void* unused, int xk, int yk, const char *text, unsigned int color)
{
  int l = text_len(NULL, (char*) text);
  render_text(NULL, xk-l, yk, text, color);
}

// -------------------------------------------------------------
// -- Render text at position Xk Yk
// -------------------------------------------------------------
void render_text (void* unused, int xk, int yk, const char *text, unsigned int color)
{
  PSPDisplayText(xk,yk,color,text);
}