#ifndef _VKEYBOARD_H
#define _VKEYBOARD_H

enum {K_1, K_3, K_5, K_7, K_9, K_PLUS, K_POUND, K_DEL, K_LEFTARROW, K_W,
K_R, K_Y, K_I, K_P, K_ASTERIX, K_RETURN, K_CTRL, K_A, K_D, K_G, K_J,
K_L, K_SEMICOLON, K_CRSRLEFTRIGHT, K_STOP, K_RSHIFT, K_X, K_V, K_N,
K_COMMA, K_SLASH, K_CRSRUPDOWN, K_SPACE, K_Z, K_C, K_B, K_M, K_DOT,
K_LSHIFT, K_F1, K_CBM, K_S, K_F, K_H, K_K, K_COLON, K_EQUAL, K_F3, K_Q,
K_E, K_T, K_U, K_O, K_AT, K_UPARROW, K_F5, K_2, K_4, K_6, K_8, K_0,
K_MINUS, K_HOME, K_F7, K_RESTORE, K_SL};

#define L_PLUS4CRS 11
#define L_C128ENTER -1
#define L_NORMAL 15
#define L_CTRL 24
#define L_PLUS4F 31
#define L_RETURN 33
#define L_SPACE 160

enum {C_YMINMAX, C_XPOS, C_END, C_KEYLEN, C_KEY };

extern unsigned char pressed_keys[256];
extern unsigned char joystick_buttons[5];

#include "bitmap.h"

void vkeyboard_init();
void vkeyboard_frame();
void vkeyboard_reset();
extern void vkeyboard_set_c64();
extern void vkeyboard_set_c128();
extern void vkeyboard_set_plus4();
extern BITMAP *gr_curkbd;
extern const int *maptable;
extern const int *keylen;
extern int kbd_numkeys;

#endif
