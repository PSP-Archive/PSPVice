#ifndef _BITMAP_H
#define _BITMAP_H

#include "types.h"

typedef struct {
  int w, h;
  const unsigned int *data;
  int stretched; // if 1 then full texture is diplayed to specified size, if 0 then texture is keep to original size and only specified part is display (cropping)
} BITMAP;

/*
typedef struct {
  int w, h;
  u32 *data;
} BITMAP2;
*/

/*
typedef struct {
  int w, h;
  unsigned char *data;
} FONTBITMAP;

typedef struct {
  int nfonts;
  int h;
  FONTBITMAP data[256];
} FONT;
*/

int text_len (void* unused, char *text);
void render_bitmap (BITMAP *bitmap, int x, int y);
void render_text (void* unused, int x, int y, const char *text, unsigned int color);
void render_text_r (void* unused, int x, int y, const char *text, unsigned int color);
void render_text_c (void* unused, int xk, int yk, int w, const char *text, unsigned int color);

#endif
