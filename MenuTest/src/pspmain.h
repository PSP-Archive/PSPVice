#ifndef _PSPMAIN_H
#define _PSPMAIN_H

void ApplyPeferences();

void          PrefSetVideoPSPDisplayMode(unsigned char value);
unsigned char PrefGetVideoPSPDisplayMode();
void          PrefSetPSPCpuSpeed(unsigned char value);
unsigned char PrefGetPSPCpuSpeed();
void          PrefSetVideoMode(unsigned char value);
void          PrefSetFontMenu(unsigned char value);
unsigned char PrefGetFontMenu();

void PSPStoreP00Filename(char* name);
char* PSPGetP00Filename();

#endif
