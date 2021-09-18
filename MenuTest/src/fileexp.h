#ifndef _FILEEXP_H
#define _FILEEXP_H

extern int file_explorer_active;

void launch_file_explorer(void (*callback)(), void *callbackparam);
void file_explorer_frame();

// File name can be 100 caracters
// Zip name can be also 100 caracters
// Path is stored is a different structure... Start part is ms0:/PSP/Games/PSPVice/
// So use 512 bytes names

typedef struct
{
  char name[256];
  char shortname[256];
  char screenshotname[256];
  char textname[256];
  unsigned char flags; // 1 = DIR
} FILEENT;

void extractname(char* result, char* source, char* ext);

#endif
