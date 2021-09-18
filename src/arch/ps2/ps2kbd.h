#ifndef _KBD_H
#define _KBD_H

void ps2_keyboard_init();
void keyboard_read();

extern const unsigned char *usbkeymap;
extern const unsigned char *usbkeyval;
extern int usbkeymaplen;


#endif
