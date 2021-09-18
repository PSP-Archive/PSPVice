// Manage link between PSP buttons and emulators keys
// Principle :
// Read PSP Buttons
// Apply key mapping and enable PSPVice correponding pushed keys
// The others functions can then call that module to know if a fonction is available are not

#include "PSPInputs.h"
#include <pspctrl.h> 
#include "pad.h"
#include <pspkernel.h>
extern char path_root[];

// PSPVice internal Keys
// 0 to 99 : C64 keybaord jeys (0 to 65)
// 100 to 199 : Joysticks keys
// 200 to 255 : PSPVice functions
// Type is : PSPIputs_value

// PSP buttons :
// Pad.c read the PSP controller values :
// unsigned int paddata = 0; // Pressed
// unsigned int new_pad = 0; // Just pressed
// unsigned int new_rel = 0; // Just released
// struct padButtonStatus buttons; // Analog stick
// PSP Have 12 buttons (Analog stick is same a digital right pad)
/*
PSP_CTRL_START
PSP_CTRL_SELECT
PSP_CTRL_LTRIGGER
PSP_CTRL_RTRIGGER
PSP_CTRL_TRIANGLE
PSP_CTRL_CROSS
PSP_CTRL_SQUARE
PSP_CTRL_CIRCLE
PSP_CTRL_UP
PSP_CTRL_DOWN
PSP_CTRL_LEFT
PSP_CTRL_RIGHT
*/

#define PSPKEYSNUMBER     12
#define PSPVICEKEYSNUMBER 255

// Define Mapping
// 12 PSP buttons -> 12 PSPVice functions
unsigned char CurrentKeyMapping[PSPKEYSNUMBER];
unsigned char SavedKeyMapping[PSPKEYSNUMBER]; // Use to save keymapping when using 2 players on same psp mode

unsigned char CurrentPressedStates[PSPVICEKEYSNUMBER];
unsigned char CurrentJustPressedStates[PSPVICEKEYSNUMBER];
unsigned char CurrentJustReleasedStates[PSPVICEKEYSNUMBER];
unsigned char PreviousPressedStates[PSPVICEKEYSNUMBER];



char gAutofire=0; // 0=None 1=Normal 2=Fast
int  autofiretimer=0; // Number of frame to wait before generating an impulsion

#define AUTOFIRE_NORMAL 15
#define AUTOFIRE_FAST   7

// Define an array with name + value (for menu)
// This is more convenient in menu to parse keys in alphabetic order
struct PSPInputs_name_struct PSPInputs_AllNames[NBTOTALCOMMANDS]=
{
  { "A",pspinp_A },
  { "B",pspinp_B },
  { "C",pspinp_C },
  { "D",pspinp_D },
  { "E",pspinp_E },
  { "F",pspinp_F },
  { "G",pspinp_G },
  { "H",pspinp_H },
  { "I",pspinp_I },
  { "J",pspinp_J },
  { "K",pspinp_K },
  { "L",pspinp_L },
  { "M",pspinp_M },
  { "N",pspinp_N },
  { "O",pspinp_O },
  { "P",pspinp_P },
  { "Q",pspinp_Q },
  { "R",pspinp_R },
  { "S",pspinp_S },
  { "T",pspinp_T },
  { "U",pspinp_U },
  { "V",pspinp_V },
  { "W",pspinp_W },
  { "X",pspinp_X },
  { "Y",pspinp_Y },
  { "Z",pspinp_Z },
  { "0",pspinp_0 },
  { "1",pspinp_1 },
  { "2",pspinp_2 },
  { "3",pspinp_3 },
  { "4",pspinp_4 },
  { "5",pspinp_5 },
  { "6",pspinp_6 },
  { "7",pspinp_7 },
  { "8",pspinp_8 },
  { "9",pspinp_9 },
  { "F1",pspinp_F1 },  
  { "F3",pspinp_F3 },  
  { "F5",pspinp_F5 }, 
  { "F7",pspinp_F7 }, 
  { "Plus",pspinp_PLUS },  
  { "Pound",pspinp_POUND },  
  { "Del",pspinp_DEL },
  { "Left Arrow",pspinp_LEFTARROW },
  { "Up Arrow",pspinp_UPARROW },
  { "Asterix",pspinp_ASTERIX }, 
  { "Return",pspinp_RETURN },
  { "CTRL",pspinp_CTRL },
  { "Semi colon",pspinp_SEMICOLON },
  { "Cursor LeftRight",pspinp_CRSRLEFTRIGHT },
  { "STOP",pspinp_STOP },
  { "CBM",pspinp_CBM },
  { "Restore",pspinp_RESTORE },
  { "Right Shift",pspinp_RSHIFT },
  { "Left Shift",pspinp_LSHIFT }, 
  { "Comma",pspinp_COMMA },
  { "Slash",pspinp_SLASH }, 
  { "Cursor UpDown",pspinp_CRSRUPDOWN },
  { "Space",pspinp_SPACE },
  { "Dot",pspinp_DOT },
  { "Colon",pspinp_COLON },
  { "Equal",pspinp_EQUAL }, 
  { "At",pspinp_AT },
  { "Minus",pspinp_MINUS },
  { "Home",pspinp_HOME },
  { "SL",pspinp_SL },
  { "Joystick1 Up",pspinp_J1_UP },
  { "Joystick1 Down",pspinp_J1_DOWN },
  { "Joystick1 Right",pspinp_J1_RIGHT },
  { "Joystick1 Left",pspinp_J1_LEFT },
  { "Joystick1 Fire",pspinp_J1_FIRE },
  { "Joystick2 Up",pspinp_J2_UP },
  { "Joystick2 Down",pspinp_J2_DOWN },
  { "Joystick2 Right",pspinp_J2_RIGHT },
  { "Joystick2 Left",pspinp_J2_LEFT },
  { "Joystick2 Fire",pspinp_J2_FIRE },
  { "Pause",pspinp_Pause },
  { "Menu",pspinp_Menu },
  { "Display Keyboard",pspinp_DisplayKeyboard },
  { "Swap Joystick",pspinp_SwapJoysticks },
  { "QuickSnapshot Save",pspinp_QuickSnapShotSave },
  { "QuickSnapshot Load",pspinp_QuickSnapShotLoad },
};

// ------------------------------------------------------------
//
// ------------------------------------------------------------
void PSPInputs_Init()
{
  CurrentKeyMapping[pspbut_Start]=pspinp_Pause;
  CurrentKeyMapping[pspbut_Select]=pspinp_Menu;
  CurrentKeyMapping[pspbut_L]=pspinp_QuickSnapShotSave;
  CurrentKeyMapping[pspbut_R]=pspinp_QuickSnapShotLoad;
  CurrentKeyMapping[pspbut_Cross]=pspinp_J1_FIRE;
  CurrentKeyMapping[pspbut_Triangle]=pspinp_DisplayKeyboard;
  CurrentKeyMapping[pspbut_Square]=pspinp_SPACE;
  CurrentKeyMapping[pspbut_Circle]=pspinp_RETURN;
  CurrentKeyMapping[pspbut_Up]=pspinp_J1_UP;
  CurrentKeyMapping[pspbut_Down]=pspinp_J1_DOWN;
  CurrentKeyMapping[pspbut_Left]=pspinp_J1_LEFT;
  CurrentKeyMapping[pspbut_Right]=pspinp_J1_RIGHT;
  
  int i;
  for (i=0; i<PSPVICEKEYSNUMBER; i++)
    CurrentPressedStates[i]=0;
}

// ------------------------------------------------------------
//
// ------------------------------------------------------------
void PSPInputs_Update()
{
  int i;
  
  // Copy state to previous states
  memcpy(PreviousPressedStates,CurrentPressedStates,1*PSPVICEKEYSNUMBER);

  // Erase current state before filling array
  for (i=0; i<PSPVICEKEYSNUMBER; i++)
  {
    CurrentPressedStates[i]=0;
    CurrentJustPressedStates[i]=0;
    CurrentJustReleasedStates[i]=0;
  }

  // Read PSP values and fille current PSPViceKeys
  if ( paddata & PSP_CTRL_START )
    CurrentPressedStates[ CurrentKeyMapping[pspbut_Start] ] |= 1;
  if ( paddata & PSP_CTRL_SELECT )
    CurrentPressedStates[ CurrentKeyMapping[pspbut_Select] ] |= 1;
  if ( paddata & PSP_CTRL_LTRIGGER )
    CurrentPressedStates[ CurrentKeyMapping[pspbut_L] ] |= 1;
  if ( paddata & PSP_CTRL_RTRIGGER )
    CurrentPressedStates[ CurrentKeyMapping[pspbut_R] ] |= 1;
  if ( paddata & PSP_CTRL_TRIANGLE )
    CurrentPressedStates[ CurrentKeyMapping[pspbut_Triangle] ] |= 1;
  if ( paddata & PSP_CTRL_CROSS )
    CurrentPressedStates[ CurrentKeyMapping[pspbut_Cross] ] |= 1;
  if ( paddata & PSP_CTRL_SQUARE )
    CurrentPressedStates[ CurrentKeyMapping[pspbut_Square] ] |= 1;
  if ( paddata & PSP_CTRL_CIRCLE )
    CurrentPressedStates[ CurrentKeyMapping[pspbut_Circle] ] |= 1;
  if ( paddata & PSP_CTRL_UP )
    CurrentPressedStates[ CurrentKeyMapping[pspbut_Up] ] |= 1;
  if ( paddata & PSP_CTRL_DOWN )
    CurrentPressedStates[ CurrentKeyMapping[pspbut_Down] ] |= 1;
  if ( paddata & PSP_CTRL_LEFT )
    CurrentPressedStates[ CurrentKeyMapping[pspbut_Left] ] |= 1;
  if ( paddata & PSP_CTRL_RIGHT )
    CurrentPressedStates[ CurrentKeyMapping[pspbut_Right] ] |= 1;


  // Autofire (Only on X button )
  if ( paddata & PSP_CTRL_CROSS )
  {
    if ( gAutofire == 1 || gAutofire == 2 )
    {
      autofiretimer++;
    
      if (    ( gAutofire == 1 && autofiretimer>AUTOFIRE_NORMAL )
           || ( gAutofire == 2 && autofiretimer>AUTOFIRE_FAST ) )
      {
        CurrentPressedStates[ CurrentKeyMapping[pspbut_Cross] ] = 0; // Force 0, means FIRE !
        autofiretimer=0;
      }
    }
  }

  // Fill the "just pressed" array
  for (i=0; i<PSPVICEKEYSNUMBER; i++)
  {
     if ( CurrentPressedStates[i]==1 && PreviousPressedStates[i]==0 )
      CurrentJustPressedStates[i] = 1;
      
     if ( CurrentPressedStates[i]==0 && PreviousPressedStates[i]==1 )
      CurrentJustReleasedStates[i] = 1;
  }

}

// ------------------------------------------------------------
//
// ------------------------------------------------------------
unsigned char PSPInputs_IsButtonPressed(unsigned char value)
{
  return CurrentPressedStates[value];
}

// ------------------------------------------------------------
//
// ------------------------------------------------------------
unsigned char PSPInputs_IsButtonJustPressed(unsigned char value)
{
  return CurrentJustPressedStates[value];
}

// ------------------------------------------------------------
//
// ------------------------------------------------------------
unsigned char PSPInputs_IsButtonJustReleased(unsigned char value)
{
  return CurrentJustReleasedStates[value];
}

// ------------------------------------------------------------
//
// ------------------------------------------------------------
unsigned char PSPInputs_GetMappingValue(unsigned char pspkey )
{
  return CurrentKeyMapping[pspkey];
}

// ------------------------------------------------------------
//
// ------------------------------------------------------------
void PSPInputs_SetMappingValue(unsigned char pspkey, unsigned char value )
{
  CurrentKeyMapping[pspkey]=value;
}

// ------------------------------------------------------------
//
// 2 players on same PSP. Save current key config
// ------------------------------------------------------------
void PSPInputs_Set2playersKeyMapping()
{
  // Save current mapping
  memcpy(SavedKeyMapping,CurrentKeyMapping,1*PSPKEYSNUMBER);
  // Set two players special mapping
  CurrentKeyMapping[pspbut_Select]=pspinp_Menu;
  CurrentKeyMapping[pspbut_Up]=pspinp_J1_UP;
  CurrentKeyMapping[pspbut_Down]=pspinp_J1_DOWN;
  CurrentKeyMapping[pspbut_Left]=pspinp_J1_LEFT;
  CurrentKeyMapping[pspbut_Right]=pspinp_J1_RIGHT;  
  CurrentKeyMapping[pspbut_L]=pspinp_J1_FIRE;
  CurrentKeyMapping[pspbut_Cross]=pspinp_J2_DOWN;
  CurrentKeyMapping[pspbut_Triangle]=pspinp_J2_UP;
  CurrentKeyMapping[pspbut_Square]=pspinp_J2_LEFT;
  CurrentKeyMapping[pspbut_Circle]=pspinp_J2_RIGHT;
  CurrentKeyMapping[pspbut_Start]=pspinp_J2_FIRE;
  CurrentKeyMapping[pspbut_R]=pspinp_J2_FIRE;
}

// ------------------------------------------------------------
//
// After 2 players key mapping restore current key mapping
// ------------------------------------------------------------
void PSPInputs_RestoreKeyMapping()
{
  // Restore Saved mapping
  memcpy(CurrentKeyMapping,SavedKeyMapping,1*PSPKEYSNUMBER);
}

// ------------------------------------------------------------
//
// ------------------------------------------------------------
unsigned char PSPInputs_GetNameIndexFromFunctionValue(unsigned char value)
{
  int i;
  for (i=0; i<NBTOTALCOMMANDS; i++)
  {
    if ( PSPInputs_AllNames[i].value == value )
      return i;
  }
  return 0;
}

// ------------------------------------------------------------
//
// ------------------------------------------------------------
void PSPInputs_Load(char* number)
{
  SceUID fid;
  char fullname[512];
  strcpy(fullname, path_root);
  strcat(fullname,"/PSPViceDatas/Keys");
  strcat(fullname,number);
  strcat(fullname,".bin");
  fid=sceIoOpen(fullname, PSP_O_RDONLY , 0777);
  if (fid>=0)
  {
    sceIoRead(fid, CurrentKeyMapping, 1*PSPKEYSNUMBER );
    sceIoClose(fid);
  } 
}

// ------------------------------------------------------------
//
// ------------------------------------------------------------
void PSPInputs_Save(char* number)
{
  SceUID fid;
  char fullname[512];
  strcpy(fullname, path_root);
  strcat(fullname,"/PSPViceDatas/Keys");
  strcat(fullname,number);
  strcat(fullname,".bin");
  fid=sceIoOpen(fullname, PSP_O_WRONLY | PSP_O_CREAT, 0777);
  if (fid>=0)
  {
    sceIoWrite(fid, CurrentKeyMapping, 1*PSPKEYSNUMBER );
    sceIoClose(fid);
  } 
}
