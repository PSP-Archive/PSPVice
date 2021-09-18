#ifndef _PSPINPUTS_H
#define _PSPINPUTS_H

// Manage link between PSP buttons and emulators keys

struct PSPInputs_name_struct
{
  char name[32];
  unsigned char value;
};

enum PSPIputs_value
{
  // C64 Keyboard
  pspinp_1 =0,  // 0
  pspinp_3 ,  
  pspinp_5 ,  
  pspinp_7 ,  
  pspinp_9 ,  
  pspinp_PLUS ,  
  pspinp_POUND ,  
  pspinp_DEL ,  
  pspinp_LEFTARROW ,  
  pspinp_W ,  // 9
  pspinp_R ,  // 10
  pspinp_Y ,  
  pspinp_I ,  
  pspinp_P ,  
  pspinp_ASTERIX ,  
  pspinp_RETURN ,  
  pspinp_CTRL ,  
  pspinp_A ,  
  pspinp_D ,  
  pspinp_G ,  
  pspinp_J , // 20
  pspinp_L ,  
  pspinp_SEMICOLON ,  
  pspinp_CRSRLEFTRIGHT ,  
  pspinp_STOP ,  
  pspinp_RSHIFT ,  
  pspinp_X ,  
  pspinp_V ,  
  pspinp_N ,   // 28
  pspinp_COMMA ,  
  pspinp_SLASH ,  
  pspinp_CRSRUPDOWN ,  
  pspinp_SPACE ,  
  pspinp_Z ,  
  pspinp_C ,  
  pspinp_B ,  
  pspinp_M ,  
  pspinp_DOT , // 37
  pspinp_LSHIFT ,  
  pspinp_F1 ,  
  pspinp_CBM ,  
  pspinp_S ,  
  pspinp_F ,  
  pspinp_H ,  
  pspinp_K ,  
  pspinp_COLON ,  
  pspinp_EQUAL ,  
  pspinp_F3 ,  
  pspinp_Q , // 48
  pspinp_E ,  
  pspinp_T ,  
  pspinp_U ,  
  pspinp_O ,  
  pspinp_AT ,  
  pspinp_UPARROW ,  
  pspinp_F5 ,  
  pspinp_2 ,  
  pspinp_4 ,  
  pspinp_6 ,  
  pspinp_8 ,  
  pspinp_0 ,  
  pspinp_MINUS ,  
  pspinp_HOME ,  
  pspinp_F7 ,  
  pspinp_RESTORE ,  
  pspinp_SL ,  // 65
  // C64 Joysticks
  pspinp_J1_UP=100,
  pspinp_J1_DOWN,
  pspinp_J1_RIGHT,
  pspinp_J1_LEFT,
  pspinp_J1_FIRE,
  pspinp_J2_UP,
  pspinp_J2_DOWN,
  pspinp_J2_RIGHT,
  pspinp_J2_LEFT,
  pspinp_J2_FIRE,
  // PSPVice functions
  pspinp_Pause=200,
  pspinp_Menu,
  pspinp_DisplayKeyboard,
  pspinp_SwapJoysticks,
  pspinp_QuickSnapShotSave,
  pspinp_QuickSnapShotLoad,
};

enum PSPinputs_PSPbuttons
{
  pspbut_Start=0,
  pspbut_Select,
  pspbut_L ,
  pspbut_R ,
  pspbut_Cross ,
  pspbut_Triangle ,
  pspbut_Square ,
  pspbut_Circle ,
  pspbut_Up ,
  pspbut_Down ,
  pspbut_Left ,
  pspbut_Right
};

#define NBTOTALCOMMANDS 82

extern struct PSPInputs_name_struct PSPInputs_AllNames[NBTOTALCOMMANDS];

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
void          PSPInputs_Load(char* number);
void          PSPInputs_Save(char* number);

#endif
