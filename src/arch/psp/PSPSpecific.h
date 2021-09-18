

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
int PSPDisplayScreenshot( char* filename, int nbchar );
void PSPGetDateAndTime(char* buffer);

extern int PSPsmoothpixel;
extern int PSPArrayVideoMode_initialised;
extern int PSPVideoMode;
extern int PSPViceVideoMode;
extern unsigned int TextureFontInitialised;
extern int PSPFontNumber;
