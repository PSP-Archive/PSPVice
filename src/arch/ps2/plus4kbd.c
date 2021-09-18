#include "vkeyboard.h"
#include "ps2kbd.h"

extern BITMAP gr_plus4kbd;

static const int plus4_keylen[] __attribute__((aligned(64))) = {
  L_PLUS4F, L_PLUS4F, L_PLUS4F, L_PLUS4F,
  L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, 
  L_CTRL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_CTRL,
  L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_RETURN,
  L_NORMAL, L_CTRL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_NORMAL, L_CTRL, L_SPACE,
  L_PLUS4CRS, L_PLUS4CRS, L_PLUS4CRS, L_PLUS4CRS
};

static const int plus4_maptable[] __attribute__((aligned(64))) = {
  C_YMINMAX, 6, 13, C_XPOS, 29,
  C_KEYLEN, L_PLUS4F, C_KEY, 0, C_XPOS, 65, C_KEY, 1, C_XPOS, 101, C_KEY, 2, C_XPOS, 137, C_KEY, 3,

  C_YMINMAX, 18, 28, C_XPOS, 10,
  C_KEYLEN, L_NORMAL, C_KEY, 4, C_KEY, 5, C_KEY, 6, C_KEY, 7, C_KEY, 8, C_KEY, 9, C_KEY, 10, C_KEY, 11, C_KEY, 12, C_KEY, 13, C_KEY, 14, C_KEY, 15, C_KEY, 16, C_KEY, 17, C_KEY, 18, C_KEY, 19,

  C_YMINMAX, 30, 40, C_XPOS, 10,
  C_KEYLEN, L_CTRL, C_KEY, 20, C_KEYLEN, L_NORMAL, C_KEY, 21, C_KEY, 22, C_KEY, 23, C_KEY, 24, C_KEY, 25, C_KEY, 26, C_KEY, 27, C_KEY, 28, C_KEY, 29, C_KEY, 30, C_KEY, 31, C_KEY, 32, C_KEY, 33, C_KEYLEN, L_CTRL, C_KEY, 34,

  C_YMINMAX, 42, 52, C_XPOS, 10,
  C_KEYLEN, L_NORMAL, C_KEY, 35, C_KEY, 36, C_KEY, 37, C_KEY, 38, C_KEY, 39, C_KEY, 40, C_KEY, 41, C_KEY, 42, C_KEY, 43, C_KEY, 44, C_KEY, 45, C_KEY, 46, C_KEY, 47, C_KEYLEN, L_RETURN, C_KEY, 48,

  C_YMINMAX, 54, 64, C_XPOS, 10,
  C_KEYLEN, L_NORMAL, C_KEY, 49, C_KEYLEN, L_CTRL, C_KEY, 50, C_KEYLEN, L_NORMAL, C_KEY, 51, C_KEY, 52, C_KEY, 53, C_KEY, 54, C_KEY, 55, C_KEY, 56, C_KEY, 57, C_KEY, 58, C_KEY, 59, C_KEY, 60, C_KEYLEN, L_CTRL, C_KEY, 61,

  C_YMINMAX, 66, 76, C_XPOS, 10,
  C_XPOS, 73, C_KEYLEN, L_SPACE, C_KEY, 62,

  C_YMINMAX, 61, 69, C_XPOS, 269, C_KEYLEN, L_PLUS4CRS, C_KEY, 63,
  C_YMINMAX, 67, 75, C_XPOS, 256, C_KEY, 64, C_XPOS, 282, C_KEY, 65,
  C_YMINMAX, 72, 80, C_XPOS, 269, C_KEY, 66,

  C_END
};

static const unsigned char plus4_usbkeyval[] = {
  0, 1, 2, 3,
  4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
  20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
  35, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
  49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,
  62,
  63, 64, 65, 66
};

static const unsigned char plus4_usbkeymap[] = {

  58, 59, 60, 61,
  41, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 45, 46, 50, 74, 42,
  43, 20, 26, 8, 21, 23, 28, 24, 12, 18, 19, 47, 73, 48,
  57, 4, 22, 7, 9, 10, 11, 13, 14, 15, 51, 52, 40,
  224, 225, 29, 27, 6, 25, 5, 17, 16, 54, 55, 56, 229,
  44,
  82, 80, 79, 81
};

void vkeyboard_set_plus4() {
  maptable = plus4_maptable;
  gr_curkbd = &gr_plus4kbd;
  keylen = plus4_keylen;
  kbd_numkeys = 67;
  usbkeymap = plus4_usbkeymap;
  usbkeyval = plus4_usbkeyval;
  usbkeymaplen = sizeof(plus4_usbkeymap);
}
