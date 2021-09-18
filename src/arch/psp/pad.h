#ifndef _PAD_H
#define _PAD_H

void pad_update();
void pad_init();

extern unsigned int new_pad;
extern unsigned int new_rel;
extern unsigned int paddata;

struct padButtonStatus
{
  unsigned char ljoy_h;
  unsigned char ljoy_v;
};

extern struct padButtonStatus buttons;

#endif
