#include "types.h"
#include <string.h>
#include "menu.h"
#include "pspmain.h"
#ifdef WIN32
#include "Win32pad.h"
#else
#include "pad.h"
#endif
#include "resources.h"
#include "lib.h"
#include "attach.h"
#include "osdmsg.h"
#include "bitmap.h"
#include "machine.h"
#include "fileexp.h"
#include "infowin.h"
#include "joystick.h"
#include "autostart.h"
#include "interrupt.h"
#include "joy.h"
#include "vkeyboard.h"

#include "log.h"

#ifdef WIN32
#include "Win32PSPSpecific.h"
#else
#include "PSPSpecific.h"
#endif
#include "PSPFileSystem.h" 

#include "PSPInputs.h"
#include "pspmain.h"
#include "PngImage.h"

#include "videoarch.h"
extern struct video_canvas_s* stored_canvas;
extern unsigned char *ps2_videobuf;
extern char gAutofire; // 0 1 2

// -- Struct for menu building

typedef struct
{
  char *name;
  int (*callback)(void *param, void *param2, int dir); // Fonction to call when selected
  void *callbackparam; // Other fonction to call
  void *callbackparam2;// Param of other fonction ... Special code : 0xFF01 for file explorer to point on system directory
  void (*draw)(void *param, int y, int sel); // Fonction to call for drawing
  void *data;
  int disabled; // 0=item is enable 1=not selectable (can be seen) 2= not visible (not seen)
  int nodir; // Enable selecting using right/left
  char* scrolling_text;
} MENUITEM;


typedef struct
{
  int (*callback)(int value, int *ptr);
  const char **selections;
  int selected;
} LISTBOX;

// -- Internal prototypes

void PSPChangeCpuFrequency(int freq);
void exit_menu();
//DMC -added quit_program function
void quit_program();

void listbox_draw(MENUITEM *m, int y, int sel);
void listbox_toggle(MENUITEM *m, void *foo, int dir);
void checkbox_draw(MENUITEM *m, int y, int sel);
void checkbox_toggle(MENUITEM *m, void *foo, int val);
void enter_submenu(MENUITEM *m, void *foo, int dir);
void leave_submenu();
void set_defaults(MENUITEM *item);

void change_true_drive_emulation(int value, int *ptr);
void attachd(char *path, int foo);
void detach_disk(int num);
void autostart(char *path);
void attachd(char *path, int foo);
void change_sid_filter(int value, int *ptr);
void change_audio(int value, int *ptr);
void show_controlswin();
void make_reset();
void load_palette(char *path, int foo);
void save_snapshot(const char *name);
void load_keysettings(const char *name);
void save_keysettings(const char *name);
void save_screenshot();
void keyboard_feed(char* string);
void save_quick_snapshot();
void load_quick_snapshot();
void display_keyboard();
void restore_keys();

extern LISTBOX sid_model;
extern LISTBOX video_psp;
extern LISTBOX video_standard;
extern LISTBOX autofire;
extern LISTBOX soundsfx;
extern LISTBOX vic20memexpansion;
extern LISTBOX menufont_psp;
extern LISTBOX cpu_psp;
extern LISTBOX mode_2_joysticks;
extern LISTBOX redefine_button_triangle;
extern LISTBOX redefine_button_square;
extern LISTBOX redefine_button_circle;
extern LISTBOX redefine_button_cross;
extern LISTBOX redefine_button_up;
extern LISTBOX redefine_button_down;
extern LISTBOX redefine_button_right;
extern LISTBOX redefine_button_left;
extern LISTBOX redefine_button_r;
extern LISTBOX redefine_button_l;
extern LISTBOX redefine_button_start;
extern int sid_filters;
extern int audio_active;
extern MENUITEM drivemenu[];
extern int is_paused;
extern int PSPViceVideoMode; // Store video mode (1=NTSC 0=PAL)
extern int PSPArrayVideoMode_initialised; // Reinit array for display
extern char* scrolling_specific_pointer; // Defined at bottom
extern char scrolling_Video_text[]; // Defined at bottom
extern char scrolling_CPUSpeed_text[]; // Defined at bottom
extern char scrolling_2Joysticks_text[]; // Defined at bottom
extern char scrolling_browse_text[]; // Defined at bottom

extern int PSPsmoothpixel;

int menu_active = 0;
MENUITEM *curmenu = (MENUITEM *) NULL;
int true_drive_emulation = 0;
int quick_snapshot = 1;

#define disp_w 480
#define disp_h 272

#define SELI (curmenu[0].disabled)
#define MNAME (curmenu[0].name)
#define SELIW 200

// Menu dimension
#define MENUBORDER 8
#define MENUW 280
#define MENUH 228
#define MENUX ((disp_w-MENUW)/2)
#define MENUY (((disp_h-MENUH)/2)-MENUBORDER)

static MENUITEM *menustack[16];
static int menustackdepth = 0;

char MenuCurrentDiskName[512]="Image"; // Store last disk or tape launched

// -------------------------------------------------------------------------------

// -- Drive Settings MENU 
// The four last lines are hidden and show only when a disk is attached.                                                                                                                                                                         Disable
MENUITEM drivemenu[] = {
  {"Disk Insertion",           (int(*)(void*, void*, int)) NULL,                 NULL,             NULL,                  (void(*)(void*, int, int)) NULL,          NULL,                                0, 1,NULL},
  {"Attach disk to unit #8",   (int(*)(void*, void*, int)) launch_file_explorer, (void*)attachd,   (void*)8,              (void(*)(void*, int, int)) NULL,          NULL,                                0, 1,NULL},
  {"Attach disk to unit #9",   (int(*)(void*, void*, int)) launch_file_explorer, (void*)attachd,   (void*)9,              (void(*)(void*, int, int)) NULL,          NULL,                                0, 1,NULL},
  {"Attach disk to unit #10",  (int(*)(void*, void*, int)) launch_file_explorer, (void*)attachd,   (void*)10,             (void(*)(void*, int, int)) NULL,          NULL,                                0, 1,NULL},
  {"Attach disk to unit #11",  (int(*)(void*, void*, int)) launch_file_explorer, (void*)attachd,   (void*)11,             (void(*)(void*, int, int)) NULL,          NULL,                                0, 1,NULL},
  {"Detach disk and Save from unit #8", (int(*)(void*, void*, int)) detach_disk,          (void*)8,         NULL,                  (void(*)(void*, int, int)) NULL,          NULL,                                2, 1,NULL},
  {"Detach disk and Save from unit #9", (int(*)(void*, void*, int)) detach_disk,          (void*)9,         NULL,                  (void(*)(void*, int, int)) NULL,          NULL,                                2, 1,NULL},
  {"Detach disk and Save from unit #10",(int(*)(void*, void*, int)) detach_disk,          (void*)10,        NULL,                  (void(*)(void*, int, int)) NULL,          NULL,                                2, 1,NULL},
  {"Detach disk and Save from unit #11",(int(*)(void*, void*, int)) detach_disk,          (void*)11,        NULL,                  (void(*)(void*, int, int)) NULL,          NULL,                                2, 1,NULL},
  {(char*) NULL}
};

// -- Sid Menu
/*
static MENUITEM sidmenu[] =
{
  {"SID Settings",   (int(*)(void*, void*, int)) NULL,            NULL, NULL,                 (void(*)(void*, int, int)) NULL, NULL, 0,0, NULL},
  {"Sound Enable",   (int(*)(void*, void*, int)) checkbox_toggle, NULL, &audio_active,        (void(*)(void*, int, int)) checkbox_draw, (void*) change_audio, 0,0,NULL},
  //{"Engine",         (int(*)(void*, void*, int)) listbox_toggle,  NULL, &sid_engine,        (void(*)(void*, int, int)) listbox_draw, NULL, 0,0,NULL},
  {"Model",          (int(*)(void*, void*, int)) listbox_toggle,  NULL, &sid_model,           (void(*)(void*, int, int)) listbox_draw, NULL, 0,0,NULL},
  //{"reSID Sampling", (int(*)(void*, void*, int)) listbox_toggle,  NULL, &sid_residsampling, (void(*)(void*, int, int)) listbox_draw, NULL, 0,0,NULL},
  {"Filters",        (int(*)(void*, void*, int)) checkbox_toggle, NULL, &sid_filters,         (void(*)(void*, int, int)) checkbox_draw, (void*) change_sid_filter, 0,0,NULL},
  {(char*)NULL}
};
*/

/*
static MENUITEM c64menu[] =
{
  {"<SID Settings>", (int(*)(void*, void*, int)) enter_submenu, sidmenu, NULL, (void(*)(void*, int, int)) NULL, NULL, 0, 1,NULL},
};
*/

// -- Save Menu
static MENUITEM savemenu[] =
{
  {"Save Game (4 slots for each game)", (int(*)(void*, void*, int)) NULL,           NULL, NULL,                                       NULL,                                  NULL, 0, 1,NULL},
  {"Do QuickSnapShot",            (int(*)(void*, void*, int)) save_quick_snapshot, NULL, NULL,            (void(*)(void*, int, int)) NULL,          NULL, 0, 1  },
  {"Restore QuickSnapShot",       (int(*)(void*, void*, int)) load_quick_snapshot, NULL, NULL,            (void(*)(void*, int, int)) NULL,          NULL, 0, 1  },
  {"Save Game on Slot1", (int(*)(void*, void*, int)) save_snapshot, (void*) "1", NULL, (void(*)(void*, int, int)) NULL, NULL, 0, 1,NULL},
  {"Save Game on Slot2", (int(*)(void*, void*, int)) save_snapshot, (void*) "2", NULL, (void(*)(void*, int, int)) NULL, NULL, 0, 1,NULL},
  {"Save Game on Slot3", (int(*)(void*, void*, int)) save_snapshot, (void*) "3", NULL, (void(*)(void*, int, int)) NULL, NULL, 0, 1,NULL},
  {"Save Game on Slot4", (int(*)(void*, void*, int)) save_snapshot, (void*) "4", NULL, (void(*)(void*, int, int)) NULL, NULL, 0, 1,NULL},
  {"Save Screenshot",             (int(*)(void*, void*, int)) save_screenshot,      NULL,             NULL,            (void(*)(void*, int, int)) NULL,          NULL, 0, 1, NULL},
  { (char*) NULL}
};

// -- Load Key Settings Menu
static MENUITEM loadkeysettingsmenu[] =
{
  {"Load key settings", (int(*)(void*, void*, int)) NULL,           NULL, NULL,                                       NULL,                                  NULL, 0, 1,NULL},
  {"Load settings 1", (int(*)(void*, void*, int)) load_keysettings, (void*) "1", NULL, (void(*)(void*, int, int)) NULL, NULL, 0, 1,NULL},
  {"Load settings 2", (int(*)(void*, void*, int)) load_keysettings, (void*) "2", NULL, (void(*)(void*, int, int)) NULL, NULL, 0, 1,NULL},
  {"Load settings 3", (int(*)(void*, void*, int)) load_keysettings, (void*) "3", NULL, (void(*)(void*, int, int)) NULL, NULL, 0, 1,NULL},
  {"Load settings 4", (int(*)(void*, void*, int)) load_keysettings, (void*) "4", NULL, (void(*)(void*, int, int)) NULL, NULL, 0, 1,NULL},
  { (char*) NULL}
};

// -- Load Key Settings Menu
static MENUITEM savekeysettingsmenu[] =
{
  {"Save key settings", (int(*)(void*, void*, int)) NULL,           NULL, NULL,                                       NULL,                                  NULL, 0, 1,NULL},
  {"Save settings 1", (int(*)(void*, void*, int)) save_keysettings, (void*) "1", NULL, (void(*)(void*, int, int)) NULL, NULL, 0, 1,NULL},
  {"Save settings 2", (int(*)(void*, void*, int)) save_keysettings, (void*) "2", NULL, (void(*)(void*, int, int)) NULL, NULL, 0, 1,NULL},
  {"Save settings 3", (int(*)(void*, void*, int)) save_keysettings, (void*) "3", NULL, (void(*)(void*, int, int)) NULL, NULL, 0, 1,NULL},
  {"Save settings 4", (int(*)(void*, void*, int)) save_keysettings, (void*) "4", NULL, (void(*)(void*, int, int)) NULL, NULL, 0, 1,NULL},
  { (char*) NULL}
};

// -- C64 Commands
static MENUITEM C64commandsmenu[] =
{
  {"C64 Pretyped command",    (int(*)(void*, void*, int)) NULL,           NULL, NULL,                                       NULL,                                  NULL, 0, 1,NULL},
  {"Load dir :           LOAD \"$\",8",  (int(*)(void*, void*, int)) keyboard_feed, (void*) "LOAD \"$\",8\r",  NULL, (void(*)(void*, int, int)) NULL, NULL, 0, 1,NULL},
  {"List dir :           LIST",          (int(*)(void*, void*, int)) keyboard_feed, (void*) "LIST:\r",         NULL, (void(*)(void*, int, int)) NULL, NULL, 0, 1,NULL},
  {"Load First program : LOAD \"*\",8,1",(int(*)(void*, void*, int)) keyboard_feed, (void*) "LOAD \"*\",8,1\r",NULL, (void(*)(void*, int, int)) NULL, NULL, 0, 1,NULL},
  {"Load prog :          LOAD\"name\",8",(int(*)(void*, void*, int)) keyboard_feed, (void*) "LOAD \"",         NULL, (void(*)(void*, int, int)) NULL, NULL, 0, 1,NULL},
  {"Run program :        RUN",           (int(*)(void*, void*, int)) keyboard_feed, (void*) "RUN:\r",          NULL, (void(*)(void*, int, int)) NULL, NULL, 0, 1,NULL},
  {"Change back color :  POKE 53280,x",  (int(*)(void*, void*, int)) keyboard_feed, (void*) "POKE 53280,",     NULL, (void(*)(void*, int, int)) NULL, NULL, 0, 1,NULL},
  {"Change border color: POKE 53281,x",  (int(*)(void*, void*, int)) keyboard_feed, (void*) "POKE 53281,",     NULL, (void(*)(void*, int, int)) NULL, NULL, 0, 1,NULL},
  { (char*) NULL}
};

// -- Video Menu
/*
static MENUITEM videomenu[] =
{
  {"Video Settings", (int(*)(void*, void*, int)) NULL,           NULL, NULL,                                       NULL,                                  NULL, 0, 1,NULL},
  {"PSP Video Mode", (int(*)(void*, void*, int)) listbox_toggle, NULL, &video_psp, (void(*)(void*,int,int)) listbox_draw, NULL, 0,0,NULL},
  {"Smooth Pixels",  (int(*)(void*, void*, int)) checkbox_toggle,NULL, &PSPsmoothpixel, (void(*)(void*, int, int)) checkbox_draw, NULL, 0,0,NULL},
  {"Video Standard", (int(*)(void*, void*, int)) listbox_toggle, NULL, &video_standard, (void(*)(void*,int,int)) listbox_draw, NULL, 0,0,NULL},
  {"Menu Font",      (int(*)(void*, void*, int)) listbox_toggle, NULL, &menufont_psp, (void(*)(void*,int,int)) listbox_draw, NULL, 0,0,NULL},
  {"<Load Palette>", (int(*)(void*, void*, int)) launch_file_explorer,  (void*)load_palette, (void*) 0xFF01, (void(*)(void*, int, int)) NULL, NULL, 0, 1},
  { (char*) NULL}
};
*/

// -- Help menu
/*
static MENUITEM helpmenu[] =
{
  {"Help", (int(*)(void*, void*, int)) NULL, NULL, NULL, (void(*)(void*, int, int)) NULL, NULL, 0, 1,NULL},
  {"Controls", (int(*)(void*, void*, int)) show_controlswin, NULL, NULL, (void(*)(void*, int, int)) NULL, NULL, 0, 1,NULL},
  {(char*) NULL}
};
*/

// -- Control menu (unused)
static MENUITEM controlsmenu[] =
{
  {"Control Settings",  (int(*)(void*, void*, int)) NULL,            NULL,   NULL,                      (void(*)(void*, int, int)) NULL, NULL, 0, 1,NULL},
  {"Triangle",          (int(*)(void*, void*, int)) listbox_toggle,  NULL,   &redefine_button_triangle, (void(*)(void*, int, int)) listbox_draw, NULL, 0, 0, NULL},
  {"Square",            (int(*)(void*, void*, int)) listbox_toggle,  NULL,   &redefine_button_square,   (void(*)(void*, int, int)) listbox_draw, NULL, 0, 0, NULL},
  {"Circle",            (int(*)(void*, void*, int)) listbox_toggle,  NULL,   &redefine_button_circle,   (void(*)(void*, int, int)) listbox_draw, NULL, 0, 0, NULL},
  {"Cross",             (int(*)(void*, void*, int)) listbox_toggle,  NULL,   &redefine_button_cross,    (void(*)(void*, int, int)) listbox_draw, NULL, 0, 0, NULL},
  {"Up",                (int(*)(void*, void*, int)) listbox_toggle,  NULL,   &redefine_button_up,       (void(*)(void*, int, int)) listbox_draw, NULL, 0, 0, NULL},
  {"Left",              (int(*)(void*, void*, int)) listbox_toggle,  NULL,   &redefine_button_left,     (void(*)(void*, int, int)) listbox_draw, NULL, 0, 0, NULL},
  {"Right",             (int(*)(void*, void*, int)) listbox_toggle,  NULL,   &redefine_button_right,    (void(*)(void*, int, int)) listbox_draw, NULL, 0, 0, NULL},
  {"Down",              (int(*)(void*, void*, int)) listbox_toggle,  NULL,   &redefine_button_down,     (void(*)(void*, int, int)) listbox_draw, NULL, 0, 0, NULL},
  {"R",                 (int(*)(void*, void*, int)) listbox_toggle,  NULL,   &redefine_button_r,        (void(*)(void*, int, int)) listbox_draw, NULL, 0, 0, NULL},
  {"L",                 (int(*)(void*, void*, int)) listbox_toggle,  NULL,   &redefine_button_l,        (void(*)(void*, int, int)) listbox_draw, NULL, 0, 0, NULL},
  {"Start",             (int(*)(void*, void*, int)) listbox_toggle,  NULL,   &redefine_button_start,    (void(*)(void*, int, int)) listbox_draw, NULL, 0, 0, NULL},
  {"<Load Key Settings>",(int(*)(void*, void*, int)) enter_submenu,        loadkeysettingsmenu,       NULL,            NULL,                                     NULL, 0, 1, NULL},
  {"<Save Key Settings>",(int(*)(void*, void*, int)) enter_submenu,        savekeysettingsmenu,        NULL,            NULL,                                     NULL, 0, 1, NULL},
  {"Restore default keys",(int(*)(void*, void*, int)) restore_keys,  NULL, NULL, (void(*)(void*, int, int)) NULL, NULL, 0, 1,NULL},
  {(char*) NULL}
};

// Settings
static MENUITEM settingsmenu[] =
{
  {"Settings",                    (int(*)(void*, void*, int)) NULL,                 NULL,             NULL,            (void(*)(void*, int, int)) NULL,          NULL, 0, 0, NULL},
#ifdef PSPVIC20
  {"Memory Expansion",            (int(*)(void*, void*, int)) listbox_toggle, NULL, &vic20memexpansion, (void(*)(void*,int,int)) listbox_draw, NULL, 0,0,NULL},
#endif
  {"True drive emulation",     (int(*)(void*, void*, int)) checkbox_toggle,      NULL,             &true_drive_emulation, (void(*)(void*, int, int)) checkbox_draw, (void*) change_true_drive_emulation, 0, 1,NULL},
  {"Sound Enable",                (int(*)(void*, void*, int)) checkbox_toggle, NULL, &audio_active,        (void(*)(void*, int, int)) checkbox_draw, (void*) change_audio, 0,0,NULL},
  {"PSP Cpu Speed",               (int(*)(void*, void*, int)) listbox_toggle,       NULL,             &cpu_psp,        (void(*)(void*,int,int))   listbox_draw,  NULL, 0, 0, scrolling_CPUSpeed_text},
  {"PSP Video Mode",              (int(*)(void*, void*, int)) listbox_toggle, NULL, &video_psp, (void(*)(void*,int,int)) listbox_draw, NULL, 0,0,NULL},
  {"Sound Stereo Effect",         (int(*)(void*, void*, int)) listbox_toggle, NULL, &soundsfx, (void(*)(void*,int,int)) listbox_draw, NULL, 0,0,NULL},
  {"Autofire (Key X)",            (int(*)(void*, void*, int)) listbox_toggle, NULL, &autofire, (void(*)(void*,int,int)) listbox_draw, NULL, 0,0,NULL},
  {"Smooth Pixels",               (int(*)(void*, void*, int)) checkbox_toggle,NULL, &PSPsmoothpixel, (void(*)(void*, int, int)) checkbox_draw, NULL, 0,0,NULL},
  {"Video Standard",              (int(*)(void*, void*, int)) listbox_toggle, NULL, &video_standard, (void(*)(void*,int,int)) listbox_draw, NULL, 0,0,NULL},
  {"Menu Font",                   (int(*)(void*, void*, int)) listbox_toggle, NULL, &menufont_psp, (void(*)(void*,int,int)) listbox_draw, NULL, 0,0,NULL},
  {"<Load Palette>",              (int(*)(void*, void*, int)) launch_file_explorer,  (void*)load_palette, (void*) 0xFF01, (void(*)(void*, int, int)) NULL, NULL, 0, 1},
  {"2 joysticks on one PSP",      (int(*)(void*, void*, int)) listbox_toggle,       NULL,             &mode_2_joysticks,(void(*)(void*, int, int)) listbox_draw, NULL, 0, 0, scrolling_2Joysticks_text},
#ifndef PSPVIC20
  {"Sid Model",                   (int(*)(void*, void*, int)) listbox_toggle,  NULL, &sid_model,           (void(*)(void*, int, int)) listbox_draw, NULL, 0,0,NULL},
  {"Sid Filters",                 (int(*)(void*, void*, int)) checkbox_toggle, NULL, &sid_filters,         (void(*)(void*, int, int)) checkbox_draw, (void*) change_sid_filter, 0,0,NULL},
#endif
  {(char*)NULL},
};

// -- Main Menu
static MENUITEM mainmenu[] =
{
  {"PSPVICE 1.1 - VICE 1.22",     (int(*)(void*, void*, int)) NULL,                 NULL,             NULL,            (void(*)(void*, int, int)) NULL,          NULL, 0, 0, NULL},
  {"Swap joysticks",              (int(*)(void*, void*, int)) checkbox_toggle,      NULL,             &swap_joysticks, (void(*)(void*, int, int)) checkbox_draw, NULL, 0, 0, NULL},
  {"<Settings>",                  (int(*)(void*, void*, int)) enter_submenu,        settingsmenu,         NULL,            NULL,                                     NULL, 0, 1, NULL},
  {"<Autostart disk/tape image>", (int(*)(void*, void*, int)) launch_file_explorer, (void*)autostart, NULL,            (void(*)(void*, int, int)) NULL,          NULL, 0, 1, scrolling_browse_text},
#ifndef PSPVIC20
  {"<Disk Insertion>",            (int(*)(void*, void*, int)) enter_submenu,        drivemenu,        NULL,            NULL,                                     NULL, 0, 1, NULL},
#endif
  {"<Keys redefinitions>",        (int(*)(void*, void*, int)) enter_submenu,        controlsmenu,     NULL,            NULL,                                     NULL, 0, 1, NULL},
  {"<Save Game>",                 (int(*)(void*, void*, int)) enter_submenu,        savemenu,         NULL,            NULL,                                     NULL, 0, 1, NULL},
#ifndef PSPVIC20
  {"<C64 commands>",              (int(*)(void*, void*, int)) enter_submenu,        C64commandsmenu,  NULL,            NULL,                                     NULL, 0, 1, NULL},
#endif
  {"Display Keyboard",            (int(*)(void*, void*, int)) display_keyboard,    NULL, NULL,            (void(*)(void*, int, int)) NULL,          NULL, 0, 1  },
  {(char*)NULL},
  {(char*)NULL},
  {(char*)NULL},
  {(char*)NULL},
  {(char*)NULL},
  {(char*)NULL}, // Need to put extra lines for the extra menus (C64 + main menu2)
};

// -- Main Menu (part 2)
static MENUITEM mainmenu2[] =
{
  {"Reset", (int(*)(void*, void*, int)) make_reset, (void*)NULL, NULL, (void(*)(void*, int, int)) NULL, NULL, 0, 1,NULL},
  {"About",                       (int(*)(void*, void*, int)) show_aboutwin_nonblock, NULL, NULL, (void(*)(void*, int, int)) NULL, NULL, 0, 1,NULL},
  {"Quit PSPVice", (int(*)(void*, void*, int)) quit_program, NULL, NULL, (void(*)(void*, int, int)) NULL, NULL, 0, 1,NULL},
};

// --------------------------------------------------------------------------
// -- Drives functions
// foo = 8 9 10 11
void attachd(char *path, int foo)
{
  if (file_system_attach_disk(foo, path)) {
    show_infowin_nonblock("Invalid image.");
  } else {
    drivemenu[foo-3].disabled = 0;
  }
  // Save name for snapshot
  strcpy(MenuCurrentDiskName, path);
}

void load_palette(char *path, int foo)
{
  int numentries;
  int i;
  //printf("Load_palette called %s %d", path, foo);

  // -- Load palette
  resources_set_value("VICIIExternalPalette", (resource_value_t)1);  // Ask for external palette to load
  resources_set_value("VICIIPaletteFile", (resource_value_t)path); // Set name of palette
  //printf("Ressources called for palette change");
  //log_warning(0,"This is a log warning");

  // -- Get palette Values
  numentries = stored_canvas->palette->num_entries;
  for (i=0; i<numentries; i++)
  {
    //printf("%s %d %d %d", stored_canvas->palette->entries[i].name, stored_canvas->palette->entries[i].red, stored_canvas->palette->entries[i].green, stored_canvas->palette->entries[i].blue );
    PSPSetPaletteEntry(i,stored_canvas->palette->entries[i].red, stored_canvas->palette->entries[i].green, stored_canvas->palette->entries[i].blue );
  }

}

void autostart(char *path)
{
  autostart_autodetect((const char*) path, (const char*) NULL, 0, AUTOSTART_MODE_RUN);
  // Save name for snapshot
  strcpy(MenuCurrentDiskName, path);
  exit_menu();
//  printf ("autostart %s\n", path);
}

void detach_disk(int num)
{
  drivemenu[num-3].disabled = 2;
  file_system_detach_disk(num);

  if (num==8)
  {
    // Save name for snapshot
    strcpy(MenuCurrentDiskName, "Image");
  }
}

void change_true_drive_emulation(int value, int *ptr)
{
  if (value == -1)
  {
    resources_get_value("DriveTrueEmulation", ptr); // First time, it is on
    // First pass, set value to off
    resources_set_value("DriveTrueEmulation", (resource_value_t)0);
    *ptr=0;
    return;
  }
  resources_set_value("DriveTrueEmulation", (resource_value_t)value);
}

// --------------------------------------------------------------------------
// -- Videos functions

// Ressource Video : -1 PAL, -2 NTSC -3 NTSCOLD
static void change_video_standard(int value, int *ptr)
{
  if (value == -1) // First time, get value and init menu
  {
    resources_get_value("MachineVideoStandard", ptr);
    *ptr = -*ptr-1;
    PSPViceVideoMode=*ptr;
    return;
  }
  
  // -- Store for PSP video display
  PSPViceVideoMode = value;
  PSPArrayVideoMode_initialised = 0;
  
  PrefSetVideoMode(value);
  
  // Change value ... value is 0 1 2 must transform into -1 -2 -3
  value = -(value+1);
  resources_set_value("MachineVideoStandard", (resource_value_t)value);

  //exit_menu();
}

// -- Change PSP Video mode
extern int PSPArrayVideoMode_initialised;
extern int PSPVideoMode;

static void change_video_psp(int value, int *ptr)
{
	//printf("%d\n", value);
	
  if (value == -1) // First time, get value and init menu
  {
    *ptr=PrefGetVideoPSPDisplayMode(); // Get preference
    value=PrefGetVideoPSPDisplayMode();
  }
  	
	PSPArrayVideoMode_initialised=0; // Force array to be reinitalised again
	PSPVideoMode=value;
	
	PrefSetVideoPSPDisplayMode(value);
}

static const char* video_standards[] = {"PAL", "NTSC", (const char*) NULL};
static const char* video_pspmodes[] = {"NORMAL", "STRETCHED", "NOBORDER", "FULLSCREEN", (const char*) NULL};

LISTBOX video_standard = { (int(*)(int, int*)) change_video_standard, video_standards, 0};
LISTBOX video_psp = { (int(*)(int, int*)) change_video_psp, video_pspmodes, 0};

static void change_menufont_psp(int value, int *ptr)
{
  if (value == -1) // First time, get value and init menu
  {
    *ptr=PrefGetFontMenu();
    value=PrefGetFontMenu();
  }
  	
  TextureFontInitialised = 0;
  PSPFontNumber=value;

  // -- Save value in prefs
	PrefSetFontMenu(value);
}

static const char* video_pspfont[] = {  "Elite_Nintendo", // 0
                                        "Flow",
                                        "Elite",
                                        "Ghost and goblins",
                                        "Gothic",
                                        "Hawks",
                                        "HD",
                                        "C64",
                                        "HexFont",
                                        "Jinx",
                                        "Master Copier", // 10
                                        "Metallion01",
                                        "Byte",
                                        "Metallion11",
                                        "Metallion12",
                                        "Metallion02",
                                        "Metallion03",
                                        "Metallion04",
                                        "Metallion05",
                                        "Metallion06",
                                        "Metallion07", // 20
                                        "Metallion08",
                                        "Cobra",
                                        "Metallion09",
                                        "Modern", // 24
                                        "Nice",
                                        "Parallax",
                                        "Peridot",
                                        "Pinochio",
                                        "Roman",
                                        "Count", // 30
                                        "System2",
                                        "TempFont",
                                        "Topaz",
                                        "Vidilores",
                                        "Defender of the Crown",
                                        "Druid", // 36
                                        (const char*) NULL };

LISTBOX menufont_psp = { (int(*)(int, int*)) change_menufont_psp, video_pspfont, 0};


// --------------------------------------------------------------------------
// -- Help functions

void show_controlswin()
{
  const char *text = "Controls\n\n\
L1: Save snapshot\n\
L2: Load snapshot\n\
SELECT: Pause\n\
START+SELECT: Reset";
  aboutwin_set_text(text, 79);
}

// --------------------------------------------------------------------------
// -- PSP Cpu functions

void change_cpu_psp(int value, int *ptr)
{
  // Value = 0 or 1 (-1 if first initialisation)

  //printf("change_cpu_psp. Value %d\n", value);

  if (value == -1) // First time, get value and init menu
  {
    *ptr=PrefGetPSPCpuSpeed(); // Get preference
    //printf("change_cpu_psp. Pref value %d\n", PrefGetPSPCpuSpeed() );
    return;
  }
	
	if (value==0)
    PSPChangeCpuFrequency(222);
    
	if (value==1)
    PSPChangeCpuFrequency(266);

	if (value==2)
    PSPChangeCpuFrequency(333);
    
  PrefSetPSPCpuSpeed(value);
}
const char* cpu_speeds[] = {"222 MHZ", "266 MHZ", "333 MHZ", (const char*) NULL};
LISTBOX cpu_psp = { (int(*)(int, int*)) change_cpu_psp, cpu_speeds, 0};

// --------------------------------------------------------------------------
// -- Autofire

// -1 0 1 2
static void change_autofire(int value, int *ptr)
{
  if (value == -1) // First time, get value and init menu
  {
    *ptr=0; // None
    value=0;
  }
	gAutofire=value;
}

static const char* autofire_modes[] = {"NONE", "NORMAL", "FAST", (const char*) NULL};
LISTBOX autofire = { (int(*)(int, int*)) change_autofire, autofire_modes, 0};

// --------------------------------------------------------------------------
// -- SoundSfx

// -1 0 1 2
static void change_soundsfx(int value, int *ptr)
{
  if (value == -1) // First time, get value and init menu
  {
    *ptr=1; // Stereo
    value=1;
  }
	pspaudio_SetEffect(value);
}

static const char* soundsfx_modes[] = {"OFF", "STEREO", "ECHO", (const char*) NULL};
LISTBOX soundsfx = { (int(*)(int, int*)) change_soundsfx, soundsfx_modes, 0};

// --------------------------------------------------------------------------
// -- 2 Joysticks functions (key mapping is changed to allow 2 players to play at same time on PSP)

void change_2_joysticks(int value, int *ptr)
{
  // Value = 0 or 1 (-1 if first initialisation)

	if (value==0)
    PSPInputs_RestoreKeyMapping();
    
	if (value==1)
    PSPInputs_Set2playersKeyMapping();

}
const char* mode_2_joysticks_list[] = {"No", "Yes", (const char*) NULL};
LISTBOX mode_2_joysticks = { (int(*)(int, int*)) change_2_joysticks, mode_2_joysticks_list, 0};

// --------------------------------------------------------------------------
// -- Redefine Keys

void change_key_mapping(int value, int *ptr, unsigned char psp_key)
{
  unsigned char realval;
  // Value = 0 or 1 (-1 if first initialisation)
  if (value==-1)
  {
    unsigned char val;
    val=PSPInputs_GetMappingValue(psp_key); // Get value of PSPVice function/key/joy mapped on that button
    val=PSPInputs_GetNameIndexFromFunctionValue(val); // Convert the value in name index
    *ptr=val; // Write it in
    return;
  }
  realval = PSPInputs_AllNames[value].value;
  PSPInputs_SetMappingValue(psp_key,realval);
}


void change_key_mapping_triangle(int value, int *ptr)
{
  change_key_mapping(value,ptr,pspbut_Triangle);
}
void change_key_mapping_square(int value, int *ptr)
{
  change_key_mapping(value,ptr,pspbut_Square);
}
void change_key_mapping_circle(int value, int *ptr)
{
  change_key_mapping(value,ptr,pspbut_Circle);
}
void change_key_mapping_cross(int value, int *ptr)
{
  change_key_mapping(value,ptr,pspbut_Cross);
}
void change_key_mapping_up(int value, int *ptr)
{
  change_key_mapping(value,ptr,pspbut_Up);
}
void change_key_mapping_down(int value, int *ptr)
{
  change_key_mapping(value,ptr,pspbut_Down);
}
void change_key_mapping_right(int value, int *ptr)
{
  change_key_mapping(value,ptr,pspbut_Right);
}
void change_key_mapping_left(int value, int *ptr)
{
  change_key_mapping(value,ptr,pspbut_Left);
}
void change_key_mapping_r(int value, int *ptr)
{
  change_key_mapping(value,ptr,pspbut_R);
}
void change_key_mapping_l(int value, int *ptr)
{
  change_key_mapping(value,ptr,pspbut_L);
}
void change_key_mapping_start(int value, int *ptr)
{
  change_key_mapping(value,ptr,pspbut_Start);
}


// ------------------
const char* redefine_button_list[] = { // 82 values
PSPInputs_AllNames[0].name,PSPInputs_AllNames[1].name,PSPInputs_AllNames[2].name,PSPInputs_AllNames[3].name,PSPInputs_AllNames[4].name,
PSPInputs_AllNames[5].name,PSPInputs_AllNames[6].name,PSPInputs_AllNames[7].name,PSPInputs_AllNames[8].name,PSPInputs_AllNames[9].name,
PSPInputs_AllNames[10].name,PSPInputs_AllNames[11].name,PSPInputs_AllNames[12].name,PSPInputs_AllNames[13].name,PSPInputs_AllNames[14].name,
PSPInputs_AllNames[15].name,PSPInputs_AllNames[16].name,PSPInputs_AllNames[17].name,PSPInputs_AllNames[18].name,PSPInputs_AllNames[19].name,
PSPInputs_AllNames[20].name,PSPInputs_AllNames[21].name,PSPInputs_AllNames[22].name,PSPInputs_AllNames[23].name,PSPInputs_AllNames[24].name,
PSPInputs_AllNames[25].name,PSPInputs_AllNames[26].name,PSPInputs_AllNames[27].name,PSPInputs_AllNames[28].name,PSPInputs_AllNames[29].name,
PSPInputs_AllNames[30].name,PSPInputs_AllNames[31].name,PSPInputs_AllNames[32].name,PSPInputs_AllNames[33].name,PSPInputs_AllNames[34].name,
PSPInputs_AllNames[35].name,PSPInputs_AllNames[36].name,PSPInputs_AllNames[37].name,PSPInputs_AllNames[38].name,PSPInputs_AllNames[39].name,
PSPInputs_AllNames[40].name,PSPInputs_AllNames[41].name,PSPInputs_AllNames[42].name,PSPInputs_AllNames[43].name,PSPInputs_AllNames[44].name,
PSPInputs_AllNames[45].name,PSPInputs_AllNames[46].name,PSPInputs_AllNames[47].name,PSPInputs_AllNames[48].name,PSPInputs_AllNames[49].name,
PSPInputs_AllNames[50].name,PSPInputs_AllNames[51].name,PSPInputs_AllNames[52].name,PSPInputs_AllNames[53].name,PSPInputs_AllNames[54].name,
PSPInputs_AllNames[55].name,PSPInputs_AllNames[56].name,PSPInputs_AllNames[57].name,PSPInputs_AllNames[58].name,PSPInputs_AllNames[59].name,
PSPInputs_AllNames[60].name,PSPInputs_AllNames[61].name,PSPInputs_AllNames[62].name,PSPInputs_AllNames[63].name,PSPInputs_AllNames[64].name,
PSPInputs_AllNames[65].name,PSPInputs_AllNames[66].name,PSPInputs_AllNames[67].name,PSPInputs_AllNames[68].name,PSPInputs_AllNames[69].name,
PSPInputs_AllNames[70].name,PSPInputs_AllNames[71].name,PSPInputs_AllNames[72].name,PSPInputs_AllNames[73].name,PSPInputs_AllNames[74].name,
PSPInputs_AllNames[75].name,PSPInputs_AllNames[76].name,PSPInputs_AllNames[77].name,PSPInputs_AllNames[78].name,PSPInputs_AllNames[79].name,
PSPInputs_AllNames[80].name,PSPInputs_AllNames[81].name,
(const char*) NULL};
LISTBOX redefine_button_triangle = { (int(*)(int, int*)) change_key_mapping_triangle, redefine_button_list, 0};
LISTBOX redefine_button_square   = { (int(*)(int, int*)) change_key_mapping_square,   redefine_button_list, 0};
LISTBOX redefine_button_circle   = { (int(*)(int, int*)) change_key_mapping_circle,   redefine_button_list, 0};
LISTBOX redefine_button_cross    = { (int(*)(int, int*)) change_key_mapping_cross,    redefine_button_list, 0};
LISTBOX redefine_button_start    = { (int(*)(int, int*)) change_key_mapping_start,    redefine_button_list, 0};
LISTBOX redefine_button_up       = { (int(*)(int, int*)) change_key_mapping_up,       redefine_button_list, 0};
LISTBOX redefine_button_down     = { (int(*)(int, int*)) change_key_mapping_down,     redefine_button_list, 0};
LISTBOX redefine_button_right    = { (int(*)(int, int*)) change_key_mapping_right,    redefine_button_list, 0};
LISTBOX redefine_button_left     = { (int(*)(int, int*)) change_key_mapping_left,     redefine_button_list, 0};
LISTBOX redefine_button_r        = { (int(*)(int, int*)) change_key_mapping_r,        redefine_button_list, 0};
LISTBOX redefine_button_l        = { (int(*)(int, int*)) change_key_mapping_l,        redefine_button_list, 0};

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
void load_keysettings(const char *name)
{
  PSPInputs_Load(name); // name = "1", "2", .. "4"
  // Reset menu
  set_defaults(controlsmenu);  
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
void save_keysettings(const char *name)
{
  PSPInputs_Save(name); // name = "1", "2", .. "4"
}

void restore_keys()
{
  PSPInputs_Init();
  // Reset menu
  set_defaults(controlsmenu);  
}

// --------------------------------------------------------------------------
// -- Snapshot functions

char snapshot_name[512]="";
char read_snapshot_name[512]="";

// ---
void save_snapshot_trap(WORD unused_address, void *unused_data)
{
  if (machine_write_snapshot(snapshot_name, 1, 1, 0)) {
    show_infowin_nonblock("Snapshot writing failed.");
  } else {
    if ( strncmp(snapshot_name, "temp",4) != 0 )
      show_info_msg("Snapshot saved", 75);
    else
      show_info_msg("Quick Snapshot Done", 75);
  }
}

// ------------------------------------------------------------
// Name : extractname
// Remove path and extension
// function is in "fileexp.c"
// ------------------------------------------------------------
/*
void extractname(char* result, char* source, char* ext)
{
    char* pTmp;
    pTmp=strrchr(source, '/');
    if (pTmp)
    {
      strcpy(result,pTmp+1);
    }
    else
    {
      strcpy(result,source);
    }
    
    // copy extension
    pTmp=strrchr(result, '.');
    
    if (ext)
    {
      ext[0]=0;
      if (pTmp)
      {
        ext[0]=pTmp[1]; // Copy ext
        ext[1]=pTmp[2];
        ext[2]=pTmp[3];
        ext[3]=0;
      }
    }

    // remove extension
    if (pTmp)
     pTmp[0]=0; // cut original string
} 
*/

// --
void CreateSnapShotName(const char* name)
{
  char* pTmp;
  // name = id of slot to save (1, 2, 3, 4), or "temp1" for quick save
  if ( strncmp(name, "temp",4) == 0 )
  {
    strcpy(snapshot_name, name); // Keep that name, will have a special treatment when saving
    return;
  }
  
  // -- Step 1, remove any directory info into name (zipped files)
  // -- Step 2, Remove extension
  extractname(snapshot_name,MenuCurrentDiskName,NULL);

  // -- Step 3, remove "_SaveX" string if any
  pTmp=strrchr(snapshot_name,'_');
  if (pTmp)
  {
    if ( strncmp(pTmp, "_Save", 5) == 0 )
      pTmp[0]=0;
  }

  // -- Build final name
  strcat(snapshot_name, "_Save");
  strcat(snapshot_name, name);
  strcat(snapshot_name, ".vsf");
  
  //printf("Saved name asked = %s\n", snapshot_name);
}

// ---
void save_snapshot(const char *name)
{
  char fullname[512];
  char* pTmp;
  char buffer[1024];
  FILE* fp;
  CreateSnapShotName(name);
  // - Set save call back  
  interrupt_maincpu_trigger_trap(save_snapshot_trap, NULL);
  if ( strncmp(name, "temp",4) == 0 )
  {
    exit_menu();
    return; // Do not save any screenshpt or txt for quick snapshot
  }
  // Save screenshot
  extractname(fullname,MenuCurrentDiskName,NULL);
  // From full name, remove any "_Savex" at end
  pTmp=strrchr(fullname,'_');
  if (pTmp)
  {
    if ( strncmp(pTmp, "_Save", 5) == 0 )
      pTmp[0]=0;
  }
  strcat(fullname, "_Save");
  strcat(fullname, name);
  strcat(fullname, ".png");
  PngSaveImage(fullname, (char*)ps2_videobuf,160,100);
  // Save date and time in txt description
  PSPGetDateAndTime(buffer); 
  extractname(fullname,MenuCurrentDiskName,NULL);
  // From full name, remove any "_Savex" at end
  pTmp=strrchr(fullname,'_');
  if (pTmp)
  {
    if ( strncmp(pTmp, "_Save", 5) == 0 )
      pTmp[0]=0;
  }
  strcat(fullname, "_Save");
  strcat(fullname, name);
  strcat(fullname, ".txt");
  fp = fopen(fullname, "wb");
  fwrite(buffer,1, strlen(buffer)+1, fp);
  fclose(fp);
  
  // Exit
  exit_menu();
}

void save_quick_snapshot()
{
  save_snapshot("temp1");
  exit_menu();
}

// ---
// PSP not used ... we use main menu to read vsf file
void read_snapshot_trap(WORD unused_address, void *unused_data)
{
  if (machine_read_snapshot(read_snapshot_name, 0)) {
    show_infowin_nonblock("Snapshot reading failed.");
  } else {
    show_info_msg("Snapshot loaded", 75);
  }
}

void read_snapshot(const char *name)
{
  strcpy(read_snapshot_name, name);
  interrupt_maincpu_trigger_trap(read_snapshot_trap, NULL);
}

void load_quick_snapshot()
{
  read_snapshot("temp1");
  exit_menu();
}

// ---



// --------------------------------------------------------------------------
// Save a small screenshot 160x100 that will be used for display of files in browser
void save_screenshot()
{
  char fullname[512];
  extractname(fullname,MenuCurrentDiskName,NULL);
  strcat(fullname, ".png");
  PngSaveImage(fullname, (char*)ps2_videobuf,160,100);
}

// --------------------------------------------------------------------------
// -- SID functions

static void change_sid_model(int value, int *ptr)
{
  if (value == -1) {
    resources_get_value("SidModel", ptr);
    return;
  }
  resources_set_value("SidModel", (resource_value_t)value);
}

static void change_sid_engine(int value, int *ptr)
{
  if (value == -1) {
    resources_get_value("SidEngine", ptr);
    return;
  }
  resources_set_value("SidEngine", (resource_value_t)value);
}

// Turn off or turn on audio
void change_audio(int value, int *ptr)
{
  if (value == -1) {
    resources_get_value("Sound", ptr);
    return;
  }
  resources_set_value("Sound", (resource_value_t)value);

  //if (value==1)
  //{
  //  pspaudio_init(); // CK : This make freeze. As this is not time consumming, let PSP sound running
  //}
}

void change_sid_filter(int value, int *ptr)
{
  if (value == -1) {
    resources_get_value("SidFilters", ptr);
    return;
  }
  resources_set_value("SidFilters", (resource_value_t)value);
}

static void change_resid_sampling(int value, int *ptr)
{
  if (value == -1) {
    resources_get_value("SidResidSampling", ptr);
    return;
  }
  resources_set_value("SidResidSampling", (resource_value_t)value);
}

const char *sid_models[] = {"6581", "8580", (const char*) NULL};
LISTBOX sid_model = { (int(*)(int, int*)) change_sid_model, sid_models, 0};

const char *sid_engines[] = {"Fast SID", "reSID", (const char*) NULL};
LISTBOX sid_engine = { (int(*)(int, int*)) change_sid_engine, sid_engines, 0};

const char *sid_residsamplings[] = {"Fast", "Interpolate", (const char*) NULL};
LISTBOX sid_residsampling = { (int(*)(int, int*)) change_resid_sampling, sid_residsamplings, 0};

int sid_filters = 0;


// --------------------------------------------------------------------------
// -- C64 Shortcuts (automatic type of lines, like "load"$",8,1"
void keyboard_feed(char* string)
{
  kbdbuf_feed(string); 
  exit_menu();
}

// -- Display C64 keyboard on screen
void display_keyboard()
{
  vkeyboard_activate();
  exit_menu();
}


// ------------------------------------------------------------------------- 
// VIC 20 Functions
#ifdef PSPVIC20

// --------------------------------------------------------------------------
// -- Autofire

// -1 0 1 2 3 4 5

enum {
    BLOCK_0 = 1,
    BLOCK_1 = 1 << 1,
    BLOCK_2 = 1 << 2,
    BLOCK_3 = 1 << 3,
    BLOCK_5 = 1 << 5
};

static void change_vic20memexpansion(int value, int *ptr)
{
  int blocks;

  if (value == -1) // First time, get value and init menu
  {
    *ptr=0;  // None
    value=0;
  }

  switch (value)
  {
    case 0:
      blocks = 0;
      break;
    case 1:
      blocks = BLOCK_0;
      break;
    case 2:
      blocks = BLOCK_1;
      break;
    case 3:
      blocks = BLOCK_1 | BLOCK_2;
      break;
    case 4:
      blocks = BLOCK_1 | BLOCK_2 | BLOCK_3;
      break;
    case 5:
      blocks = BLOCK_0 | BLOCK_1 | BLOCK_2 | BLOCK_3 | BLOCK_5;
      break;
  }
  resources_set_int("RamBlock0", blocks & BLOCK_0 ? 1 : 0);
  resources_set_int("RamBlock1", blocks & BLOCK_1 ? 1 : 0);
  resources_set_int("RamBlock2", blocks & BLOCK_2 ? 1 : 0);
  resources_set_int("RamBlock3", blocks & BLOCK_3 ? 1 : 0);
  resources_set_int("RamBlock5", blocks & BLOCK_5 ? 1 : 0);

}

static const char* vic20memexpansion_modes[] = {"NONE", "3K", "8K", "16K", "24K", "FULL", (const char*) NULL};
LISTBOX vic20memexpansion = { (int(*)(int, int*)) change_vic20memexpansion, vic20memexpansion_modes, 0};

/*

static TUI_MENU_CALLBACK(set_common_memory_configuration_callback)
{
        switch ((int)param) {
          case MEM_NONE:
            blocks = 0;
            break;
          case MEM_ALL:
            blocks = BLOCK_0 | BLOCK_1 | BLOCK_2 | BLOCK_3 | BLOCK_5;
            break;
          case MEM_3K:
            blocks = BLOCK_0;
            break;
          case MEM_8K:
            blocks = BLOCK_1;
            break;
          case MEM_16K:
            blocks = BLOCK_1 | BLOCK_2;
            break;
          case MEM_24K:
            blocks = BLOCK_1 | BLOCK_2 | BLOCK_3;
            break;
          default:
            // Shouldn't happen.
            log_debug("What?!");
            blocks = 0;         // Make compiler happy.
        }
        resources_set_int("RamBlock0", blocks & BLOCK_0 ? 1 : 0);
        resources_set_int("RamBlock1", blocks & BLOCK_1 ? 1 : 0);
        resources_set_int("RamBlock2", blocks & BLOCK_2 ? 1 : 0);
        resources_set_int("RamBlock3", blocks & BLOCK_3 ? 1 : 0);
        resources_set_int("RamBlock5", blocks & BLOCK_5 ? 1 : 0);
        ui_update_menus();
    }

    // This way, the "Not Really!" item is always the default one.
    *become_default = 0;
    return NULL;
}

static tui_menu_item_def_t common_memory_configurations_items[] = {
    { "_Not Really!",
      "Keep the current settings",
      NULL, NULL, 0, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "--" },
    { "_Unexpanded",
      "Setup a completely unexpanded VIC20",
      set_common_memory_configuration_callback, (void *)MEM_NONE, NULL,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_3K (block 0)",
      "Setup a 3K-expanded VIC20",
      set_common_memory_configuration_callback, (void *)MEM_3K, NULL,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_8K (block 1)",
      "Setup an 8K-expanded VIC20",
      set_common_memory_configuration_callback, (void *)MEM_8K, NULL,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_16K (blocks 1/2)",
      "Setup an 8K-expanded VIC20",
      set_common_memory_configuration_callback, (void *)MEM_16K, NULL,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_24K (blocks 1/2/3)",
      "Setup a 24K-expanded VIC20",
      set_common_memory_configuration_callback, (void *)MEM_24K, NULL,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_All (blocks 0/1/2/3/5)",
      "Setup a VIC20 with all the possible RAM stuffed in",
      set_common_memory_configuration_callback, (void *)MEM_ALL, NULL,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
}; 
*/

#endif

// --------------------------------------------------------------------------
// -- Menu management functions

void exit_menu()
{
  menu_active = 0;
  is_paused = 0;
}

void make_reset()
{
  machine_trigger_reset(MACHINE_RESET_MODE_HARD);
  exit_menu();
}

static void enter_menu()
{
  curmenu = mainmenu;
  menustackdepth = 0;
  joystick_clear_all();
}

static void draw_menu()
{
  int f;
  static int repeatcntr = 0;
  static int repeatspeed = 0;
  MENUITEM *menus = &curmenu[1];
  MENUITEM *m;
  int l = 0, l2 = 0;

  while (menus[SELI].disabled)
  {
    SELI++;
    if (menus[SELI].name==NULL)
      SELI = 0;
  }

  PSPSetFillColor(20, 20, 20, 192);
  PSPFillRectangle(MENUX, MENUY-MENUBORDER, MENUX+MENUW, MENUY+MENUH+MENUBORDER);
  render_text_c (NULL, MENUX, MENUY+2, MENUW, MNAME, 0xF0F0F0);
  
  #define MENUFIRSTLINE (18+6)
  
  for (m=&curmenu[1];m->name!=NULL;m++,l2++)
  {
    if (m->disabled>1)
      continue;
    if (l2 == SELI)
    {
      //PSPSetFillColor(90, 90, 90,192);
      //PSPFillRectangle(MENUX+MENUW/2-SELIW/2, MENUY+18+l*13+6, MENUX+MENUW/2+SELIW/2, MENUY+18+l*13+12+6);
    }
    if (m->draw != NULL)
    {
      m->draw(m, MENUY+MENUFIRSTLINE+l*13, l2 == SELI);
    }
    else if (m->disabled)
    {
      render_text(NULL, MENUX+MENUBORDER, MENUY+MENUFIRSTLINE+l*13, m->name, 0xA0A0A0);
    } 
    else
    {
      if (l2 == SELI)
      {
        render_text(NULL, MENUX+MENUBORDER, MENUY+MENUFIRSTLINE+l*13, m->name, 0xF0F0F0);
      }
      else
      {
        render_text(NULL, MENUX+MENUBORDER, MENUY+MENUFIRSTLINE+l*13, m->name, 0x30C070);
      }
    }
    l++;
  }
  l = l2;
  if (new_pad & PAD_TRIANGLE) {
    leave_submenu();
  }
  // -- Item is selected
  if (new_pad & PAD_CROSS)
  {
    if (menus[SELI].callback != NULL)
    {
      // If there is any specific scrolling then set it
      if ( menus[SELI].scrolling_text != NULL )
      	scrolling_specific_pointer = menus[SELI].scrolling_text;
    
      if (menus[SELI].callbackparam == NULL)
        menus[SELI].callback(&menus[SELI], NULL, 0);
      else
        menus[SELI].callback(menus[SELI].callbackparam, menus[SELI].callbackparam2, 0);
    }
  }
  
  f = 0;
  
  if (new_pad & PAD_LEFT)
  {
    f = 1;
    repeatspeed=0;
  }
  else if (paddata & PAD_LEFT)
  {
    repeatcntr++;
    if ((repeatspeed>1&&repeatcntr>7)||repeatcntr>25)
      f = 1;
  }
  
  if (f) {
    repeatcntr = 0;
    repeatspeed++;
    if (menus[SELI].callback != NULL) {
      if (!menus[SELI].nodir)
      {
        if (menus[SELI].callbackparam == NULL)
          menus[SELI].callback(&menus[SELI], NULL, 1);
        else
          menus[SELI].callback(menus[SELI].callbackparam, menus[SELI].callbackparam2, 1);
      }
    }
  }
  f = 0;
  if (new_pad & PAD_RIGHT)
  {
    f = 1;
    repeatspeed=0;
  }
  else if (paddata & PAD_RIGHT)
  {
    repeatcntr++;
    if ((repeatspeed>1&&repeatcntr>7)||repeatcntr>25)
      f = 1;
  }
  if (f)
  {
    repeatcntr = 0;
    repeatspeed++;
    if (menus[SELI].callback != NULL) {
      if (!menus[SELI].nodir)
      {
        if (menus[SELI].callbackparam == NULL)
          menus[SELI].callback(&menus[SELI], NULL, 2);
        else
          menus[SELI].callback(menus[SELI].callbackparam, menus[SELI].callbackparam2, 2);
      }
    }
  }
  f = 0;
  if (new_pad & PAD_UP) {
    f = 1;
    repeatspeed=0;
  } else if (paddata & PAD_UP) {
    repeatcntr++;
    if ((repeatspeed>1&&repeatcntr>3)||repeatcntr>18)
      f = 1;
  }
  if (f) {
    repeatcntr = 0;
    repeatspeed++;
    SELI--;
    if (SELI<0)
      SELI = l-1;
    while (menus[SELI].disabled)
    {
      SELI--;
      if (SELI<0)
        SELI = l-1;
    }
  }
  f = 0;
  if (new_pad & PAD_DOWN) {
    f = 1;
    repeatspeed=0;
  } else if (paddata & PAD_DOWN) {
    repeatcntr++;
    if ((repeatspeed>1&&repeatcntr>3)||repeatcntr>18)
      f = 1;
  }
  if (f) {
    repeatcntr = 0;
    repeatspeed++;
    SELI++;
    if (SELI>=l)
      SELI = 0;
    while (menus[SELI].disabled)
    {
      SELI++;
      if (SELI>=l)
        SELI = 0;
    }
  }
}

// ----------------------------------------------------------------
int menu_update()
{
  static int resettriggered = 0;
  // Already in menu
  if (menu_active)
  {
    if (new_rel & PAD_SELECT) // Exit menu
    {
      menu_active = 0;
      is_paused=0;
    }
    draw_menu();
    return 1;
  }
  // Not in menu (in game)
  if (resettriggered)
  {
    if (!(paddata & PAD_START || paddata & PAD_SELECT)) {
      resettriggered = 0;
    }
  }
  else
  {
    // Test if we can enter menu
    if (    (new_rel & PAD_SELECT) && (is_paused==0) // I do not remap this key, too dangerous
         || PSPInputs_IsButtonJustReleased(pspinp_Menu) ) 
    {
      enter_menu();
      menu_active = 1;
      is_paused=1; // Emulator is paused when entering menu
    }
    // Pause asked ?
    if (PSPInputs_IsButtonJustPressed(pspinp_Pause) )
    {
      is_paused = !is_paused;
    }
    // Reset asked ?
    if ( (paddata & PAD_START) && (paddata & PAD_SELECT) )
    {
      machine_trigger_reset(MACHINE_RESET_MODE_HARD);
      resettriggered = 1;
    }
  }

  if (quick_snapshot)
  {
    if ( PSPInputs_IsButtonJustPressed(pspinp_QuickSnapShotLoad) )
    {
      read_snapshot("temp1");
    }
    if ( PSPInputs_IsButtonJustPressed(pspinp_QuickSnapShotSave) )
    {
      save_snapshot("temp1");
    }
  }
  return 0;
}

// ----------------------------------------------------------------
void checkbox_toggle(MENUITEM *m, void *foo, int val)
{
  if (val == 0)        /* Cross pressed from the pad? */
    *((int*)(m->callbackparam2)) = !*((int*)(m->callbackparam2));
  if (m->data) {  /* Does callback function (for notifying changes) exist? */
    ((void (*)(int))m->data)(*((int*)(m->callbackparam2)));
  }
}

// ----------------------------------------------------------------
void checkbox_draw(MENUITEM *m, int y, int sel)
{
  char str2[32];
  
  if (*((int*)(m->callbackparam2)))
    sprintf (str2,"[X]");
  else
    sprintf (str2,"[ ]");
  
  if (sel)
  {
    render_text   (NULL, MENUX+MENUBORDER, y,       m->name, 0xF0F0F0);
    render_text_r (NULL, MENUX+MENUW-MENUBORDER, y, str2,    0x3030e0);
  }
  else
  {
    render_text   (NULL, MENUX+MENUBORDER, y,       m->name, 0x3030e0);
    render_text_r (NULL, MENUX+MENUW-MENUBORDER, y, str2,    0x3030e0);
  }
}

// ----------------------------------------------------------------
void listbox_toggle(MENUITEM *m, void *foo, int dir)
{
  if (dir == 0 || dir == 2) {
    ((LISTBOX*)(m->callbackparam2))->selected++;
    if (((LISTBOX*)(m->callbackparam2))->selections[((LISTBOX*)(m->callbackparam2))->selected]==NULL)
      ((LISTBOX*)(m->callbackparam2))->selected = 0;
  } else {
    ((LISTBOX*)(m->callbackparam2))->selected--;
    if (((LISTBOX*)(m->callbackparam2))->selected < 0) {
      ((LISTBOX*)(m->callbackparam2))->selected++;
      while (((LISTBOX*)(m->callbackparam2))->selections[((LISTBOX*)(m->callbackparam2))->selected]!=NULL)
        ((LISTBOX*)(m->callbackparam2))->selected++;
      ((LISTBOX*)(m->callbackparam2))->selected--;
    }
  }
  ((LISTBOX*)(m->callbackparam2))->callback(((LISTBOX*)(m->callbackparam2))->selected, (int*) NULL);
}

// ----------------------------------------------------------------
void listbox_draw(MENUITEM *m, int y, int sel)
{
  render_text   (NULL, MENUX+MENUBORDER, y, m->name, sel?0xF0F0F0:0x30C070);
  render_text_r (NULL, MENUX+MENUW-MENUBORDER, y, ((LISTBOX*)(m->callbackparam2))->selections[((LISTBOX*)(m->callbackparam2))->selected], 0x3030e0);
}

// ----------------------------------------------------------------
static void add_menuitem(MENUITEM *dest, int len, MENUITEM *src, int srclen)
{
  int i, j;
  // Search first NULL element
  for (i=0;i<len/sizeof(MENUITEM);i++)
    if (dest[i].name == NULL)
      break;
  // If that first free element show that this is not enough room for adding all menus then exit
  if (i > len/sizeof(MENUITEM)-srclen/sizeof(MENUITEM)-1)
    return;
  // All is ok, add all new menus lines
  for (j=0;j<srclen/sizeof(MENUITEM);j++)
    memcpy(&dest[i+j], &src[j], sizeof(MENUITEM));
  dest[i+j].name = (char*) NULL;
}

// ----------------------------------------------------------------
void set_defaults(MENUITEM *item)
{
  int i=0;
  while (item[i].name!=NULL) {
    if ((void (*)(int))item[i].callback == (void (*)(int))listbox_toggle) // If list choice, then call callback function with value -1
    {
      ((LISTBOX*)(item[i].callbackparam2))->callback(-1, &((LISTBOX*)(item[i].callbackparam2))->selected);
    }
    if ((void (*)(int))item[i].callback == (void (*)(int))checkbox_toggle && item[i].data != NULL)
      ((void (*)(int, int*))item[i].data)(-1, (int*) item[i].callbackparam2);
    i++;
  } 
}

// ----------------------------------------------------------------
void enter_submenu(MENUITEM *m, void *foo, int dir)
{
  menustack[menustackdepth++] = curmenu;
  curmenu = m;
}

// ----------------------------------------------------------------
void leave_submenu()
{
  if (menustackdepth<=0)
  {
    menu_active = 0;
    is_paused=0;
    return;
  }
  curmenu = menustack[--menustackdepth];
}

// ------------------------------------------------------------------
//
// This is menu init
// For initialising default values, menu need to be called by hand
// ------------------------------------------------------------------
void set_machine_menu(int machine)
{
  //printf("Set machine menu\n");
  /*
  switch (machine)
  {
    case MENU_C64:
      add_menuitem(mainmenu, sizeof(mainmenu), c64menu, sizeof(c64menu));
      set_defaults(sidmenu);
      break;
  }
  */
  //set_defaults(sidmenu);
  add_menuitem(mainmenu, sizeof(mainmenu), mainmenu2, sizeof(mainmenu2));
  //set_defaults(videomenu);
  set_defaults(mainmenu);
  set_defaults(controlsmenu);
  set_defaults(drivemenu);
  set_defaults(settingsmenu);
}

// ------------------------------------------------------------------
char scrolling_main_text[] = "Welcome to PSPVice V1.1. This emulator is brought to you by Christophe Kohler. Thanks to VICE team for emulator core development. PSPVice is based on PS2Vice by Rami Rasanen. Others credits fly to : C64 keyboard gfx by WiDDy and Shabanak, PSP Sound code by Pop Kid, True drive emulation debug by Dennis Crews. Thanks to all testers : Kjetil, Vicenzo, Darren, Richard, Yann and all I forgot. \0";
int scrolling_main_text_count=0;

char* scrolling_specific_pointer=NULL; // Null if inactive. This pointer is used to overpass the main scrolling and play a different one, when finished, it comes back to normal scrolling

char scrolling_Video_text[]      = "... Video settings ... Use UP and DOWN arrow to choose line and CROSS to change values...\0";
char scrolling_CPUSpeed_text[]   = "... Select here PSP cpu speed. 333 is best but use a bit more battery ...\0";
char scrolling_2Joysticks_text[] = "... 2 Players simultaneously on one PSP. Second player play with TRI/SQUA/CIRCL/CROSS and R/start for fire. First player fire with L and moce using left pad or analog stick. You can swap joysticks ...\0";
char scrolling_browse_text[] = "... Press SQUARE or CIRCLE to go up. Recognised file types are d64, t64, prg, tap, vsf, g64, p00. Text info files and game base's NFO file are detected and displayed. Files can be zipped, multiple files per zip supported...\0";

char scrolling_current_buffer[62] = "                                                             ";
int scrolling_current_value=0; // When more than 8, then need a new caracter

// ------------------------------------------------------------------
//
// ------------------------------------------------------------------
void scrolling_update()
{
	int i;

	scrolling_current_value++;
	
	// -- Check if we need to add a letter
	if ( scrolling_current_value >= 8 )
	{
		scrolling_current_value -= 8; // Need to add a letter
		
		// -- Shift all letter, from 0 to 60 (61 is null caracter)
		for (i=0; i<60; i++)
		{
			scrolling_current_buffer[i]=scrolling_current_buffer[i+1];
		}

		// -- Add letter from current scrolling	
		// Check if we register a specific scrolling
		if ( scrolling_specific_pointer != NULL)
		{
			// Take caracter from specific scrolling
			scrolling_current_buffer[60]=scrolling_specific_pointer[0];
			scrolling_specific_pointer++;
			if ( scrolling_specific_pointer[0]==0)
				scrolling_specific_pointer=NULL;
		}
		else
		{
			// Take caracter from main scrolling
			scrolling_current_buffer[60]=scrolling_main_text[scrolling_main_text_count];
			scrolling_main_text_count++;
			if ( scrolling_main_text[scrolling_main_text_count]==0 )
				scrolling_main_text_count=0;
		}
	}
}

// ------------------------------------------------------------------
//
// ------------------------------------------------------------------
void scrolling_display()
{
  // Display Background
  PSPSetFillColor(20, 20, 20, 192);
  PSPFillRectangle(0, 272-8-8-1, 480, 272-8+1);

  // Display text
  PSPDisplayText(0-scrolling_current_value, 272-8-8, 0xFFFFFFFF, scrolling_current_buffer);
}

// DMC added function
void quit_program()
{
//	machine_shutdown();
	archdep_close_default_log_file();
	SavePreferences();
//	sceGuTerm();
//	machine_shutdown();
	sceKernelExitGame();
	
}
