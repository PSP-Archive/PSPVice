#include <tamtypes.h>
#include <stdio.h>
#include "cdvd_rpc.h"
#include "cdvd_vsync.h"
#include "support.h"

#define STOP_VSYNCCOUNT 800

int cdvd_initialized = 0;
static int cdvd_accessed = 0;

void cdvd_vsync() {
  if (!cdvd_initialized)
    return;
  if (cdvd_accessed<STOP_VSYNCCOUNT) {
    if (++cdvd_accessed==STOP_VSYNCCOUNT) {
      CDVD_Stop();
      CDVD_FlushCache();
    }
  }
}

void cdvd_access() {
  if (!cdvd_initialized)
    cdvd_initialize();
  cdvd_accessed = 0;
}

void cdvd_initialize() {
  loadmodule_robust("cdvd.irx");
  CDVD_Init();
  CDVD_FlushCache();
  cdvd_initialized = 1;
  printf ("CDVD INITIALIZED\n");
}
