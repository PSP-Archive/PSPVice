#include <kernel.h>
#include <compat.h>
#include <stdio.h>
#include <tamtypes.h>
#include <malloc.h>
#include "libpad.h"
#include "loadmodule.h"
#include "pad.h"
#include "hw.h"

u32 paddata = 0;
u32 paddata1 = 0;
u32 paddata2 = 0;
u32 new_pad = 0;
u32 new_pad1 = 0;
u32 new_pad2 = 0;
u32 new_rel = 0;
u32 new_rel1 = 0;
u32 new_rel2 = 0;
u32 old_pad = 0;
u32 old_pad1 = 0;
u32 old_pad2 = 0;


struct padButtonStatus buttons;
struct padButtonStatus buttons1;
struct padButtonStatus buttons2;

void pad_loadmodules() {
 int ret;
 if((ret =_lf_bind(0)) != 0)
   printf("_lf_bind: %d\n", ret);

  ret = _sifLoadModule("rom0:SIO2MAN", 0, NULL, 0);
  if (ret < 0) {
    printf("sifLoadModule sio failed: %d\n", ret);
  }
  ret = _sifLoadModule("rom0:PADMAN", 0, NULL, 0);
  if (ret < 0) {
    printf("sifLoadModule pad failed: %d\n", ret);
  }
}

void pad_init() {
  unsigned char *padbuf = memalign(64, 256);
  unsigned char *padbuf2 = memalign(64, 256);
  int ret;

  buttons1.btns[0]=buttons1.btns[1]=0xFF;
  buttons2.btns[0]=buttons2.btns[1]=0xFF;
  buttons1.ljoy_h = buttons1.ljoy_v = 127;
  buttons2.ljoy_h = buttons2.ljoy_v = 127;

  install_VRstart_handler();
  pad_loadmodules();
  padInit(0);
  if((ret = padPortOpen(0, 0, padbuf)) == 0) {
    printf("padOpenPort failed: %d\n", ret);
  }
  if((ret = padPortOpen(1, 0, padbuf2)) == 0) {
    printf("padOpenPort failed: %d\n", ret);
  }
}

void pad_update() {
  int tmp, s;
  static int inited1 = 0;
  static int inited2 = 0;
  static int brokencnt1 = 0;
  static int brokencnt2 = 0;
  static int updatecnt = 49;

  if (!inited1 && !brokencnt1 ) {
    padSetMainMode(0, 0, 1, 3);
    inited1 = 1;
  }

  if (!inited2 && !brokencnt2) {
    padSetMainMode(1, 0, 1, 3);
    inited2 = 1;    
  }

  if (updatecnt++>50) {
    updatecnt = 0;
  }

  if ((s=padGetState(0, 0))!=6) {
    if (brokencnt1 > 25) {
      if (s==2) {   /* Normal digital pad */
        padRead(0, 0, &buttons1);
      } else {
        buttons1.btns[0] = buttons1.btns[1] = 0xFF;
      }
      buttons1.ljoy_h = buttons1.ljoy_v = 127;
      inited1 = 0;
    } else {
      brokencnt1++;
    }
  } else {
    padRead(0, 0, &buttons1);
    brokencnt1=0;
  }
  if ((s=padGetState(1, 0))!=6) {
    if (brokencnt2 > 25) {
      if (s==2) {   /* Normal digital pad */
        padRead(1, 0, &buttons2);
      } else {
        buttons2.btns[0] = buttons2.btns[1] = 0xFF;
      }
      buttons2.ljoy_h = buttons2.ljoy_v = 127;
      inited2 = 0;
    } else {
      brokencnt2++;
    }
  } else {
    padRead(1, 0, &buttons2);
    brokencnt2 = 0;
  }

  tmp = (buttons1.ljoy_h+buttons2.ljoy_h-127);
  if (tmp < 0)
    buttons.ljoy_h = 0;
  else if (tmp > 255)
    buttons.ljoy_h = 255;
  else
    buttons.ljoy_h = tmp;

  tmp = (buttons1.ljoy_v+buttons2.ljoy_v-127);
  if (tmp < 0)
    buttons.ljoy_v = 0;
  else if (tmp > 255)
    buttons.ljoy_v = 255;
  else
    buttons.ljoy_v = tmp;

  paddata1 = 0xffff ^ ((buttons1.btns[0] << 8) | buttons1.btns[1]);
  paddata2 = 0xffff ^ ((buttons2.btns[0] << 8) | buttons2.btns[1]);
  paddata = paddata1 | paddata2;
  new_pad = paddata & ~old_pad;
  new_rel = ~paddata & old_pad;
  new_pad1 = paddata1 & ~old_pad1;
  new_rel1 = ~paddata1 & old_pad1;
  new_pad2 = paddata2 & ~old_pad2;
  new_rel2 = ~paddata2 & old_pad2;
  old_pad = paddata;
  old_pad1 = paddata1;
  old_pad2 = paddata2;
}
