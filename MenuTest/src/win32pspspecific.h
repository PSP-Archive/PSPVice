#include "stdio.h"

// Redefine filsystem function

#define fopen  Myfopen
#define fread  Myfread
#define fwrite Myfwrite
#define ftell  Myftell
#define fflufh Myfflufh
#define fclose Myfclose
#define fseek  Myfseek
#define fputc  Myfputc
#define fgetc  Myfgetc

#define malloc  Mymalloc
#define free    Myfree

#define __attribute__(x)

void PSPDisplayImage(int x, int y, void* bitmap);
void PSPDisplayImage_Reset();
void PSPDisplayText(int x,  int y, unsigned int color, const char* text);
void PSPFillFileArray();
void PSPStartDisplay();
void PSPDisplayVideoBuffer();
void PSPWaitVBL();
void PSPSwap();
void PSPEndDisplay();
int  PSPGetVBLCount();
void PSPDisplayString(int x, int y, unsigned int color, char* string);
void PSPSetFillColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A);
void PSPFillRectangle(unsigned int x, unsigned int y, unsigned int x2, unsigned int y2);
int  PSPDisplayScreenshot( char* filename, int nbchar );
void PSPGetDateAndTime(char* buffer);

extern int PSPsmoothpixel;
extern int PSPArrayVideoMode_initialised;
extern int PSPVideoMode;
extern int PSPViceVideoMode;
extern unsigned int TextureFontInitialised;
extern int PSPFontNumber;

extern char path_root[2048]; // Store name of current dir (passed to main as argument). Should be (ms0:/PSP/Game/PSPVice)
extern char path_games[2048]; // Store games directories 

void PSPSetPaletteEntry(int id, unsigned char red,unsigned char green, unsigned char blue);

void pspaudio_init(void);
void pspaudio_clear(void);
void pspaudio_write(short* buffer, int num_samples);
void pspaudio_shutdown(void);
void pspaudio_SetEffect(int value); 

void          PSPInputs_Init();
void          PSPInputs_Update();
unsigned char PSPInputs_IsButtonPressed(unsigned char value);
unsigned char PSPInputs_IsButtonJustPressed(unsigned char value);
unsigned char PSPInputs_IsButtonJustReleased(unsigned char value);
unsigned char PSPInputs_GetMappingValue(unsigned char pspkey);
void          PSPInputs_SetMappingValue(unsigned char pspkey, unsigned char value);
void          PSPInputs_Set2playersKeyMapping();
void          PSPInputs_RestoreKeyMapping();
unsigned char PSPInputs_GetNameIndexFromFunctionValue(unsigned char value);
void          PSPInputs_Load(const char* number);
void          PSPInputs_Save(const char* number); 

void sceKernelExitGame(void);
void SavePreferences(void);
void kbdbuf_feed(char*);
void archdep_close_default_log_file(void);

void sceGuTexFlush();
void sceKernelDcacheWritebackAll();
void sceDisplayWaitVblankStart();
void sceDisplayGetFrameBuf(void** a, int* b, int* c, int* d);

#define BYTE unsigned char

#define feof fendoffile // for PSP (do not work)
