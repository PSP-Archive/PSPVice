#include <tamtypes.h>
#include "bitmap.h"

static const u32 gr_button3_[35*13] __attribute__((aligned(64))) = {
  0x00FF8000, 0x00FF8000, 0x00FF8000, 0x00FF8000,
  0x00FF8000, 0x00FF8000, 0x00FF8000, 0x00FF8000,
  0x00FF8000, 0x00FF8000, 0x00FF8000, 0x00FF8000,
  0x00FF8000, 0x00FF8000, 0x00FF8000, 0x00FF8000,
  0x00FF8000, 0x00FF8000, 0x00FF8000, 0x00FF8000,
  0x00FF8000, 0x00FF8000, 0x00FF8000, 0x00FF8000,
  0x00FF8000, 0x00FF8000, 0x00FF8000, 0x00FF8000,
  0x00FF8000, 0x00FF8000, 0x00FF8000, 0x00FF8000,
  0x00FF8000, 0x00FF8000, 0x00FF8000, 0x00FF8000,
  0x00FF8000, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x00FF8000, 0x00FF8000, 0x00FF8000, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x00FF8000, 0x00FF8000, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x00FF8000,
  0x00FF8000, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x00FF8000, 0x00FF8000,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x00FF8000, 0x00FF8000, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x00FF8000, 0x00FF8000, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x00FF8000,
  0x00FF8000, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x00FF8000, 0x00FF8000,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x00FF8000, 0x00FF8000, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x00FF8000, 0x00FF8000, 0x00FF8000, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x504080FF, 0x504080FF,
  0x504080FF, 0x504080FF, 0x00FF8000, 0x00FF8000,
  0x00FF8000, 0x00FF8000, 0x00FF8000, 0x00FF8000,
  0x00FF8000, 0x00FF8000, 0x00FF8000, 0x00FF8000,
  0x00FF8000, 0x00FF8000, 0x00FF8000, 0x00FF8000,
  0x00FF8000, 0x00FF8000, 0x00FF8000, 0x00FF8000,
  0x00FF8000, 0x00FF8000, 0x00FF8000, 0x00FF8000,
  0x00FF8000, 0x00FF8000, 0x00FF8000, 0x00FF8000,
  0x00FF8000, 0x00FF8000, 0x00FF8000, 0x00FF8000,
  0x00FF8000, 0x00FF8000, 0x00FF8000, 0x00FF8000,
  0x00FF8000, 0x00FF8000, 0x00FF8000
};

BITMAP gr_button3 = {
    w: 35,
    h: 13,
 data: gr_button3_
};