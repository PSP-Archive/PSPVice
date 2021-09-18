/*
 * PSP Software Development Kit - http://www.pspdev.org
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPSDK root for details.
 *
 * Copyright (c) 2005 Jesper Svennevid
 */

#include "vice.h"

#include <malloc.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

#include <stdlib.h> // Atoi

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <pspgu.h>
#include <pspgum.h>

#include <stdarg.h> 

#include "joy.h"
#include "resources.h"

#include "PSPSpecific.h"
#include "PSPInputs.h"

#include "PngImage.h"
#include "ZipSupport.h"

#include "unzip.h"

#define NOPRINTF

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)
#define PIXEL_SIZE (4) // change this if you change to another screenmode
#define FRAME_SIZE (BUF_WIDTH * SCR_HEIGHT * PIXEL_SIZE)
#define ZBUF_SIZE (BUF_WIDTH SCR_HEIGHT * 2) // zbuffer seems to be 16-bit?

// Define the module info section
PSP_MODULE_INFO("PSPVice", 0, 1, 0); 

unsigned int list[262144] __attribute__((aligned(16)));

char path_prefix[1024];

extern FILE *tempfile[4];

char path_root[2048]; // Store name of current dir (passed to main as argument). Should be (ms0:/PSP/Game/PSPVice)
char path_games[2048]; // Store games directories

// --------------------------------------------------------------------------
// Prototypes

void ApplyPeferences();
void waitNSecond(int n);

// --------------------------------------------------------------------------

int SetupCallbacks();

#define MYPRINTF_NB_LINES 30

char MyPrintfArray[MYPRINTF_NB_LINES][61];

int in_gameloop; // Use for debug display

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Store path name of elf, this is root dir base
  char* p;
  strcpy(path_root, argv[0]);
	p = strrchr(path_root, '/'); // Remove elf name
	if (p)
	  *p = 0;
  // Copy this path for games directory. This one will change (browse from menu)
  strcpy(path_games, path_root);
  strcat(path_games, "/games/"); // With / at the end
  
  in_gameloop=0; // Debug

	SetupCallbacks();

	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG); 
	
	// setup GU

	sceGuInit();
	
	sceGuStart(GU_DIRECT,list);
	sceGuDrawBuffer(GU_PSM_8888,(void*)0,BUF_WIDTH);
	sceGuDispBuffer(SCR_WIDTH,SCR_HEIGHT,(void*)FRAME_SIZE,BUF_WIDTH);
	sceGuDepthBuffer((void*)(FRAME_SIZE*2),BUF_WIDTH);
	sceGuOffset(2048 - (SCR_WIDTH/2),2048 - (SCR_HEIGHT/2));
	sceGuViewport(2048,2048,SCR_WIDTH,SCR_HEIGHT);
	sceGuDepthRange(0xc350,0x2710);
	sceGuScissor(0,0,SCR_WIDTH,SCR_HEIGHT);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuDisable(GU_ALPHA_TEST); 
  sceGuDisable(GU_DEPTH_TEST);
  sceGuColor(0xFFFFFFFF);
	sceGuClearDepth(0);
	sceGuClearStencil(0); 	

	sceGuFinish();
	sceGuSync(0,0);

	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE); 
	
	// -- Load settings
  LoadPreferences();
  ApplyPeferences();
	
  // -- Run emulator

  PSPDisplayImage_Reset();
  PSPStartDisplay();
  #ifndef PSPVIC20
  PSPDisplayString(8*(60-7)/2,  8*(30/2) ,    0x00ffffff, "PSPVICE"); 
  PSPDisplayString(8*(60-23)/2, 8*(30/2+1) , 0x00ffffff, "Loading, please wait..."); 
  #else
  PSPDisplayString(8*(60-8)/2,  8*(30/2) ,    0x00ffffff, "PSPVIC20"); 
  PSPDisplayString(8*(60-23)/2, 8*(30/2+1) , 0x00ffffff, "Loading, please wait..."); 
  PSPDisplayString(8, 8*30, 0x00ffffff, "Note : If your game does not want to launch, try to change");
  PSPDisplayString(8, 8*31, 0x00ffffff, "memory configuration. Go into Settings/Memory Expansion");
  PSPDisplayString(8, 8*32, 0x00ffffff, "and change value. Most games runs with NONE, 3K or FULL.");
  #endif
  PSPEndDisplay();
  PSPWaitVBL();
  PSPSwap();

  printf("Start\n");

  main_program(0, NULL); 

  printf("End\n");

  //while(1)
  //{
  //}
  
  waitNSecond(5);

	sceGuTerm();

	sceKernelExitGame();
	return 0;
}

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
// Exit callback
int exit_callback(int arg1, int arg2, void *common)
{
  // -- Restore clock frequency
	scePowerSetClockFrequency(222,222,111);

  // -- Save settings
  SavePreferences();

  // Save log
  /*
  SceUID fid;
  fid=sceIoOpen("ms0:/PSP/log1", PSP_O_WRONLY | PSP_O_CREAT, 0777);
  if (fid>=0)
  {
    sceIoWrite(fid, logck, LOGSIZE*2);
    sceIoClose(fid);
  } 
  */

	sceKernelExitGame();
	return 0;
}

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
// Callback thread
int CallbackThread(SceSize args, void *argp)
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);

	sceKernelSleepThreadCB();

	return 0;
}

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
// Sets up the callback thread and returns its thread id
int SetupCallbacks(void)
{
	int thid = 0;

	thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
} 

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------

void main_exit(void)
{
/*
    log_message(LOG_DEFAULT, "\nExiting...");

    machine_shutdown();

    putchar ('\n');
*/
}


// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
/*
int MyPrintf(const char * string, ...)
{ 
    char localstring[256];
    va_list ap;
    va_start(ap, string);
    vsprintf(localstring, string, ap);
    pspDebugScreenPrintf(localstring);
    return 0; 
}
*/

// ------------------------------------------------
// ------------------------------------------------


void AddOneLine(char* localstring)
{
    int i,j;

    // All lines go up
	  for (i=1; i<MYPRINTF_NB_LINES; i++)
	  {
	    for(j=0; j<61; j++)
	    {
	      MyPrintfArray[i-1][j]=MyPrintfArray[i][j];
	    }
	  }
    
    // Erase last line
    for (i=0; i<61; i++)
      MyPrintfArray[MYPRINTF_NB_LINES-1][i]=0;

    // If string is NULL then display "NULL"
    if (localstring[0]==0)
    {
      localstring[0]='N';
      localstring[1]='U';
      localstring[2]='L';
      localstring[3]='L';
      localstring[4]=0;
    }
    
    // Copy string into last line
    j=0;

    while(localstring[j]!=0 && j<60)
    {
      MyPrintfArray[MYPRINTF_NB_LINES-1][j]=localstring[j];
      j++;
    }
}

// ------------------------------------------------
// ------------------------------------------------
int MyPrintf(const char * string, ...)
{
    #ifdef NOPRINTF
    return 0;
    #endif

    int i,j;
    int size;
    int loop;
    va_list ap;

    char localstring[256];

    #ifdef PRINTF_ONLY_AT_LOADING
    if (in_gameloop)
      return;
    #endif
    
    for (i=0; i<256; i++)
      localstring[i]=0;

    size = strlen(string);
    if (size>256)
    {
      sprintf(localstring,"LINE TOO LONG. CAN NOT DISPLAY");
    }
    else
    {
  	  va_start(ap, string);
      vsprintf(localstring, string, ap);
    }
    
    size = strlen(localstring);
    loop=(size/60)+1;

    for (i=0; i<loop; i++)
    {
      AddOneLine(localstring+(i*60) );
    }

    // Force display of the whole array, only if emulation has not started yet
    if (in_gameloop==0)
    {
      PSPDisplayImage_Reset();
    
      PSPStartDisplay();

  	  for (i=0; i<MYPRINTF_NB_LINES; i++)
  	  {
    	  PSPDisplayString(8,  8+(i*8), 0x00ffffff, MyPrintfArray[i]); 
  	  }

      // ---------------
      PSPEndDisplay();
      PSPWaitVBL();
      PSPSwap();
    }

  	va_end(ap);  
  	
  	// If a button is pressed then stop display (to be able to read)
  	
  	SceCtrlData		buf;  
  	sceCtrlReadBufferPositive(&buf, 1); 
  	while ( buf.Buttons )
  	{
    	sceCtrlReadBufferPositive(&buf, 1); 
    };
    
    	  	   
}


// -- Display on screen printf
void DisplayDebugMessages()
{
  #ifdef NOPRINTF
  return;
  #endif

	// If a button is pressed do not display anything (better for debug)
	SceCtrlData		buf;  
	sceCtrlReadBufferPositive(&buf, 1); 
	if ( buf.Buttons )
    return 0;
      
  int i;
  for (i=0; i<MYPRINTF_NB_LINES; i++)
  {
	  PSPDisplayString(8,  8+(i*8), 0x00ffffff, MyPrintfArray[i]); 
  }
}


// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------

struct PSPVicePreferences_type
{
  unsigned char VideoSmooth;              // 0 1 = Off on
  unsigned char VideoPSPDisplayMode;      // 0 = Normal mode, 1=FULL SCREEN - STRECHED 2=FULLSCREEN NO BORDER 3=FULL SCREEN - STRECHED - NOBORDER
  unsigned char PSPCpuSpeed;              // 0=222 1=266 2=333
  unsigned char JoystickSwap;             // 0 1 = Off On
  unsigned char VideoMode;                // 0=PAL 1=NTSC
  char          VideoPaletteName[128];    //
  unsigned char VideoFonteMenu;           // 0 to n. Selected font in menu
  unsigned char ControlSelectedKeySquare; //
  unsigned char ControlSelectedKeyCircle; //
} PSPVicePreferences_type;

struct PSPVicePreferences_type PSPVicePreferences;
// --------------------------------------------------------------------------
void          PrefSetVideoPSPDisplayMode(unsigned char value) { PSPVicePreferences.VideoPSPDisplayMode=value; }
unsigned char PrefGetVideoPSPDisplayMode() { return PSPVicePreferences.VideoPSPDisplayMode; }
void          PrefSetPSPCpuSpeed(unsigned char value) { PSPVicePreferences.PSPCpuSpeed=value; }
unsigned char PrefGetPSPCpuSpeed() { return PSPVicePreferences.PSPCpuSpeed; }
void          PrefSetVideoMode(unsigned char value) { PSPVicePreferences.VideoMode=value; }
void          PrefSetFontMenu(unsigned char value) { PSPVicePreferences.VideoFonteMenu=value; }
unsigned char PrefGetFontMenu() { return PSPVicePreferences.VideoFonteMenu; }

// --------------------------------------------------------------------------
// Name : SavePreferences
// --------------------------------------------------------------------------
void SavePreferences()
{
  // -- Update some values
  PSPVicePreferences.VideoSmooth=PSPsmoothpixel;
  PSPVicePreferences.JoystickSwap=swap_joysticks;
  
  // -- Save struct on disk
  // name "prefs.bin"
  // Save log
  SceUID fid;
  char fullname[2048];
  strcpy(fullname, path_root);
  strcat(fullname,"/PSPViceDatas/prefs.bin");
  fid=sceIoOpen(fullname, PSP_O_WRONLY | PSP_O_CREAT, 0777);
  if (fid>=0)
  {
    sceIoWrite(fid, &PSPVicePreferences, sizeof(PSPVicePreferences_type) );
    sceIoClose(fid);
  } 

}

// --------------------------------------------------------------------------
// Name : LoadAndApplyPreferences
// --------------------------------------------------------------------------
void LoadPreferences()
{
  // -- Check on memory stick if there is any preference file
  // If yes load it
  SceUID fid;
  char fullname[2048];
  strcpy(fullname, path_root);
  strcat(fullname,"/PSPViceDatas/prefs.bin");
  fid=sceIoOpen(fullname, PSP_O_RDONLY , 0777);
  if (fid>=0)
  {
    sceIoRead(fid, &PSPVicePreferences, sizeof(PSPVicePreferences_type) );
    sceIoClose(fid);
  } 
  else  
  {
    // If no, put default preferences
    PSPVicePreferences.VideoSmooth=1;
    PSPVicePreferences.VideoPSPDisplayMode=3; // 3=FULL SCREEN - STRECHED - NOBORDER
    PSPVicePreferences.JoystickSwap = 1;      // Start with Joystick Swapped
    PSPVicePreferences.PSPCpuSpeed = 2;       // 333 Mhz
    PSPVicePreferences.VideoMode = 0;         // 0 = Pal 1=Ntsc
    PSPVicePreferences.VideoFonteMenu=24;     // Default font (Modern)
  }
}

// --------------------------------------------------------------------------
// Name : ApplyPeferences
// --------------------------------------------------------------------------
void ApplyPeferences()
{
  // -- Smooth pixel
  PSPsmoothpixel = PSPVicePreferences.VideoSmooth;

	// -- PSP Video mode
	PSPArrayVideoMode_initialised=0; // Force array to be reinitalised again
	PSPVideoMode=PSPVicePreferences.VideoPSPDisplayMode;

  // -- Swap Joystick
  swap_joysticks = PSPVicePreferences.JoystickSwap;

  // -- Cpu Speed
	if (PSPVicePreferences.PSPCpuSpeed==0)
    PSPChangeCpuFrequency(222);
	else if (PSPVicePreferences.PSPCpuSpeed==1)
    PSPChangeCpuFrequency(266);
	else if (PSPVicePreferences.PSPCpuSpeed==2)
    PSPChangeCpuFrequency(333);  

  // -- Video mode
  PSPViceVideoMode = PSPVicePreferences.VideoMode;
  PSPArrayVideoMode_initialised = 0;
  //resources_set_value("MachineVideoStandard", (resource_value_t)-(PSPVicePreferences.VideoMode+1)); // Change value ... value is 0 1 2 must transform into -1 -2 -3

  // -- Set font
  PSPFontNumber=PSPVicePreferences.VideoFonteMenu;
  
  // -- Set Key Mapping
  PSPInputs_Init();

}

// --------------------------------------------------------------------------
// Name : waitNSecond
// --------------------------------------------------------------------------
void waitNSecond(int n)
{
	int x;
	for (x = 0; x < 60*n; x++) sceDisplayWaitVblankStart();
}


// --------------------------------------------------------------------------
// Name : waitNSecond
// --------------------------------------------------------------------------
char LastP00SelectedFilename[2048];

// --------------------------------------------------------------------------
// Name : 
// --------------------------------------------------------------------------
void PSPStoreP00Filename(char* name)
{
  strcpy(LastP00SelectedFilename, name);
}

// --------------------------------------------------------------------------
// Name : 
// --------------------------------------------------------------------------
char* PSPGetP00Filename()
{
  return LastP00SelectedFilename;
}