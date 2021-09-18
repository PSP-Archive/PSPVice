#ifndef _SUPPORT_H
#define _SUPPORT_H

int loadmodule_robust();
FILE *fopen_robust(const char *name, const char *mode);
int fio_open_robust(char *name, int mode);

#endif
