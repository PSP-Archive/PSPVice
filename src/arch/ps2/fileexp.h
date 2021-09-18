#ifndef _FILEEXP_H
#define _FILEEXP_H

extern int file_explorer_active;

void launch_file_explorer(int (*callback)(), void *callbackparam);
void file_explorer_frame();

#endif
