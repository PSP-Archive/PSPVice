#include <tamtypes.h>
#include <kernel.h>
#include <string.h>
#include <gs.h>
#include <g2.h>
#include "menu.h"
#include "ps2main.h"
#include "libpad.h"
#include "pad.h"
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

typedef struct {
  char *name;
  int (*callback)(void *param, void *param2, int dir);
  void *callbackparam;
  void *callbackparam2;
  void (*draw)(void *param, int y, int sel);
  void *data;
  int disabled;
  int nodir;
} MENUITEM;

typedef struct {
  int (*callback)(int value, int *ptr);
  const char **selections;
  int selected;
} LISTBOX;

extern int is_paused;

extern FONT f_arial;

int menu_active = 0;
static MENUITEM *curmenu = NULL;

extern MENUITEM drivemenu[];

static int true_drive_emulation = 0;
static int quick_snapshot = 1;

#define SELI (curmenu[0].disabled)
#define MNAME (curmenu[0].name)
#define SELIW 200
#define MENUW 250
#define MENUH 200
#define MENUX ((disp_w-MENUW)/2)
#define MENUY ((disp_h-MENUH)/2)


static MENUITEM *menustack[16];
static int menustackdepth = 0;

void exit_menu();

static void enter_submenu(MENUITEM *m, void *foo, int dir) {
  menustack[menustackdepth++] = curmenu;
  curmenu = m;
}

static void leave_submenu() {
  if (menustackdepth<=0) {
    menu_active = 0;
    return;
  }
  curmenu = menustack[--menustackdepth];
}

static void attachd(char *path, int foo) {
  if (file_system_attach_disk(foo, path)) {
    show_infowin_nonblock("Invalid image.");
  } else {
    drivemenu[foo-2].disabled = 0;
  }
}

static void autostart(char *path) {
  autostart_autodetect(path, NULL, 0, AUTOSTART_MODE_RUN);
  exit_menu();
//  printf ("autostart %s\n", path);
}

static void detach_disk(int num) {
  drivemenu[num-2].disabled = 2;
  file_system_detach_disk(num);
}

static void checkbox_toggle(MENUITEM *m, void *foo, int val) {
  if (val == 0)        /* Cross pressed from the pad? */
    *((int*)(m->callbackparam2)) = !*((int*)(m->callbackparam2));
  if (m->data) {  /* Does callback function (for notifying changes) exist? */
    ((void (*)(int))m->data)(*((int*)(m->callbackparam2)));
  }
}

static void checkbox_draw(MENUITEM *m, int y, int sel) {
  char str[512];
  if (*((int*)(m->callbackparam2)))
    snprintf (str, sizeof(str), "%s [\xFD\x40\xC5\x80\xFF\xFC]", m->name);
  else
    snprintf (str, sizeof(str), "%s [\xFE]", m->name);
  if (sel) 
    render_text_c(&f_arial, MENUX, y, MENUW, str, 0xF0F0F0);
  else
    render_text_c(&f_arial, MENUX, y, MENUW, str, 0x30C070);
}

static void listbox_toggle(MENUITEM *m, void *foo, int dir) {
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
  ((LISTBOX*)(m->callbackparam2))->callback(((LISTBOX*)(m->callbackparam2))->selected, NULL);
}

static void listbox_draw(MENUITEM *m, int y, int sel) {
  render_text (&f_arial, MENUX+30, y, m->name, sel?0xF0F0F0:0x30C070);
  render_text_r (&f_arial, MENUX+MENUW-30, y, ((LISTBOX*)(m->callbackparam2))->selections[((LISTBOX*)(m->callbackparam2))->selected], 0x3030e0);
}

static void change_true_drive_emulation(int value, int *ptr) {
  if (value == -1) {
    resources_get_value("DriveTrueEmulation", ptr);
    return;
  }
  resources_set_value("DriveTrueEmulation", (resource_value_t)value);
}

static MENUITEM drivemenu[] = {
  {"Drive Settings", NULL, NULL, NULL, NULL, NULL, 0, 1},
  {"True drive emulation", checkbox_toggle, NULL, &true_drive_emulation, checkbox_draw, change_true_drive_emulation, 0, 1},
  {"Attach disk to unit #8", launch_file_explorer, attachd, (void*)8, NULL, NULL, 0, 1},
  {"Attach disk to unit #9", launch_file_explorer, attachd, (void*)9, NULL, NULL, 0, 1},
  {"Attach disk to unit #10", launch_file_explorer, attachd, (void*)10, NULL, NULL, 0, 1},
  {"Attach disk to unit #11", launch_file_explorer, attachd, (void*)11, NULL, NULL, 0, 1},
  {"Detach disk from unit #8", detach_disk, (void*)8, NULL, NULL, NULL, 2, 1},
  {"Detach disk from unit #9", detach_disk, (void*)9, NULL, NULL, NULL, 2, 1},
  {"Detach disk from unit #10", detach_disk, (void*)10, NULL, NULL, NULL, 2, 1},
  {"Detach disk from unit #11", detach_disk, (void*)11, NULL, NULL, NULL, 2, 1},
  {NULL}
};

static void change_video_standard(int value, int *ptr) {
  if (value == -1) {
    resources_get_value("MachineVideoStandard", ptr);
    *ptr = -*ptr-1;
    return;
  }
  value = -(value+1);
  resources_set_value("MachineVideoStandard", (resource_value_t)value);
}

static const char *video_standards[] = {"PAL", "NTSC", NULL};
static LISTBOX video_standard = {change_video_standard, video_standards, 0};

static MENUITEM videomenu[] = {
  {"Video Settings", NULL, NULL, NULL, NULL, NULL, 0, 1},
  {"Video Standard", listbox_toggle, NULL, &video_standard, listbox_draw, NULL, 0},
  {NULL}
};

void show_controlswin() {
  const char *text = "Controls\n\n\
L1: Save snapshot\n\
L2: Load snapshot\n\
SELECT: Pause\n\
START+SELECT: Reset";
  aboutwin_set_text(text, 79);
}

static MENUITEM helpmenu[] = {
  {"Help", NULL, NULL, NULL, NULL, NULL, 0, 1},
  {"About", show_aboutwin_nonblock, NULL, NULL, NULL, NULL, 0, 1},
  {"Controls", show_controlswin, NULL, NULL, NULL, NULL, 0, 1},
  {NULL}
};

static MENUITEM controlsmenu[] = {
  {"Control Settings", NULL, NULL, NULL, NULL, NULL, 0, 1},
  {"Swap joysticks", checkbox_toggle, NULL, &swap_joysticks, checkbox_draw, NULL, 0},
  {"Snapshot with L1/L2", checkbox_toggle, NULL, &quick_snapshot, checkbox_draw, NULL, 0},
  {NULL}
};

static MENUITEM mainmenu[] = {
  {"VICE 1.14", NULL, NULL, NULL, NULL, NULL, 0},
  {"Autostart disk/tape image...", launch_file_explorer, autostart, NULL, NULL, NULL, 0, 1},
  {"Drive Settings", enter_submenu, drivemenu, NULL, NULL, NULL, 0, 1},
  {"Video Settings", enter_submenu, videomenu, NULL, NULL, NULL, 0, 1},
  {"Control Settings", enter_submenu, controlsmenu, NULL, NULL, NULL, 0, 1},
  {NULL},
  {NULL},
  {NULL},
  {NULL},
  {NULL},
  {NULL},
};

void exit_menu() {
  menu_active = 0;
}

static MENUITEM mainmenu2[] = {
  {"Reset", machine_trigger_reset, (void*)MACHINE_RESET_MODE_HARD, NULL, NULL, NULL, 0, 1},
  {"Help", enter_submenu, helpmenu, NULL, NULL, NULL, 0, 1},
  {"Exit menu", exit_menu, NULL, NULL, NULL, NULL, 0, 1},
};

static void enter_menu() {
  curmenu = mainmenu;
  menustackdepth = 0;
  joystick_clear_all();
}

static void draw_menu() {
  int f;
  static int repeatcntr = 0;
  static int repeatspeed = 0;
  MENUITEM *menus = &curmenu[1];
  MENUITEM *m;
  int l = 0, l2 = 0;

  while (menus[SELI].disabled) {
    SELI++;
    if (menus[SELI].name==NULL)
      SELI = 0;
  }

  g2_set_fill_color(20, 20, 20);
  g2_fill_rect(MENUX, MENUY, MENUX+MENUW, MENUY+MENUH);
  render_text_c (&f_arial, MENUX, MENUY+1, MENUW, MNAME, 0xF0F0F0);
  g2_set_fill_color(0, 0, 0);

  for (m=&curmenu[1];m->name!=NULL;m++,l2++) {
    if (m->disabled>1)
      continue;
    if (l2 == SELI) {
      g2_set_fill_color(90, 90, 90);
      g2_fill_rect(MENUX+MENUW/2-SELIW/2, MENUY+18+l*13+6, MENUX+MENUW/2+SELIW/2, MENUY+18+l*13+12+6);
      g2_set_fill_color(0, 0, 0);
    }
    if (m->draw != NULL) {
      m->draw(m, MENUY+18+l*13, l2 == SELI);
    } else if (m->disabled) {
      render_text_c(&f_arial, MENUX, MENUY+18+l*13, MENUW, m->name, 0xA0A0A0);
    } else {
      if (l2 == SELI) {
        render_text_c(&f_arial, MENUX, MENUY+18+l*13, MENUW, m->name, 0xF0F0F0);
      } else {
        render_text_c(&f_arial, MENUX, MENUY+18+l*13, MENUW, m->name, 0x30C070);
      }
    }
    l++;
  }
  l = l2;
  if (new_pad & PAD_TRIANGLE) {
    leave_submenu();
  }
  if (new_pad & PAD_CROSS) {
    if (menus[SELI].callback != NULL) {
      if (menus[SELI].callbackparam == NULL)
        menus[SELI].callback(&menus[SELI], NULL, 0);
      else
        menus[SELI].callback(menus[SELI].callbackparam, menus[SELI].callbackparam2, 0);
    }
  }
  f = 0;
  if (new_pad & PAD_LEFT) {
    f = 1;
    repeatspeed=0;
  } else if (paddata & PAD_LEFT) {
    repeatcntr++;
    if ((repeatspeed>1&&repeatcntr>7)||repeatcntr>25)
      f = 1;
  }
  if (f) {
    repeatcntr = 0;
    repeatspeed++;
    if (menus[SELI].callback != NULL) {
      if (!menus[SELI].nodir) {
        if (menus[SELI].callbackparam == NULL)
          menus[SELI].callback(&menus[SELI], NULL, 1);
        else
          menus[SELI].callback(menus[SELI].callbackparam, menus[SELI].callbackparam2, 1);
      }
    }
  }
  f = 0;
  if (new_pad & PAD_RIGHT) {
    f = 1;
    repeatspeed=0;
  } else if (paddata & PAD_RIGHT) {
    repeatcntr++;
    if ((repeatspeed>1&&repeatcntr>7)||repeatcntr>25)
      f = 1;
  }
  if (f) {
    repeatcntr = 0;
    repeatspeed++;
    if (menus[SELI].callback != NULL) {
      if (!menus[SELI].nodir) {
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
    while (menus[SELI].disabled) {
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
    while (menus[SELI].disabled) {
      SELI++;
      if (SELI>=l)
        SELI = 0;
    }
  }
}

static char snapshot_name[512]="";
static char read_snapshot_name[512]="";

static void save_snapshot_trap(WORD unused_address, void *unused_data) {
  if (machine_write_snapshot(snapshot_name, 1, 1, 0)) {
    show_infowin_nonblock("Snapshot writing failed.");
  } else {
    show_info_msg("Snapshot saved", 75);
  }
}

static void save_snapshot(const char *name) {
  strcpy(snapshot_name, name);
  interrupt_maincpu_trigger_trap(save_snapshot_trap, NULL);
}

static void read_snapshot_trap(WORD unused_address, void *unused_data) {
  if (machine_read_snapshot(read_snapshot_name, 0)) {
    show_infowin_nonblock("Snapshot reading failed.");
  } else {
    show_info_msg("Snapshot loaded", 75);
  }
}

static void read_snapshot(const char *name) {
  strcpy(read_snapshot_name, name);
  interrupt_maincpu_trigger_trap(read_snapshot_trap, NULL);
}

int menu_update() {
  static int resettriggered = 0;
  if (menu_active) {
    if (new_rel & PAD_START) {
      menu_active = 0;
    }
    draw_menu();
    return 1;
  }
  if (resettriggered) {
    if (!(paddata & PAD_START || paddata & PAD_SELECT)) {
      resettriggered = 0;
    }
  } else {
    if (new_rel & PAD_START) {
      enter_menu();
      menu_active = 1;
    }
    if (new_rel & PAD_SELECT) {
      is_paused = !is_paused;
    }
    if (paddata & PAD_START && paddata & PAD_SELECT) {
      machine_trigger_reset(MACHINE_RESET_MODE_HARD);
      resettriggered = 1;
    }
  }
  if (quick_snapshot) {
    if (new_pad & PAD_L2) {
      read_snapshot("temp1");
    }
    if (new_pad & PAD_L1) {
      save_snapshot("temp1");
    }
  }
  return 0;
}

static void change_sid_model(int value, int *ptr) {
  if (value == -1) {
    resources_get_value("SidModel", ptr);
    return;
  }
  resources_set_value("SidModel", (resource_value_t)value);
}

static void change_sid_engine(int value, int *ptr) {
  if (value == -1) {
    resources_get_value("SidEngine", ptr);
    return;
  }
  resources_set_value("SidEngine", (resource_value_t)value);
}

static void change_sid_filter(int value, int *ptr) {
  if (value == -1) {
    resources_get_value("SidFilters", ptr);
    return;
  }
  resources_set_value("SidFilters", (resource_value_t)value);
}

static void change_resid_sampling(int value, int *ptr) {
  if (value == -1) {
    resources_get_value("SidResidSampling", ptr);
    return;
  }
  resources_set_value("SidResidSampling", (resource_value_t)value);
}

static const char *sid_models[] = {"6581", "8580", NULL};
static LISTBOX sid_model = {change_sid_model, sid_models, 0};

static const char *sid_engines[] = {"Fast SID", "reSID", NULL};
static LISTBOX sid_engine = {change_sid_engine, sid_engines, 0};

static const char *sid_residsamplings[] = {"Fast", "Interpolate", NULL};
static LISTBOX sid_residsampling = {change_resid_sampling, sid_residsamplings, 0};

static int sid_filters = 0;

static MENUITEM sidmenu[] = {
  {"SID Settings", NULL, NULL, NULL, NULL, NULL, 0},
  {"Engine", listbox_toggle, NULL, &sid_engine, listbox_draw, NULL, 0},
  {"Model", listbox_toggle, NULL, &sid_model, listbox_draw, NULL, 0},
  {"reSID Sampling", listbox_toggle, NULL, &sid_residsampling, listbox_draw, NULL, 0},
  {"Filters", checkbox_toggle, NULL, &sid_filters, checkbox_draw, change_sid_filter, 0},
  {NULL}
};

static MENUITEM c64menu[] = {
  {"SID Settings", enter_submenu, sidmenu, NULL, NULL, NULL, 0, 1},
};

static void add_menuitem(MENUITEM *dest, int len, MENUITEM *src, int srclen) {
  int i, j;
  for (i=0;i<len/sizeof(MENUITEM);i++)
    if (dest[i].name == NULL)
      break;
  if (i > len/sizeof(MENUITEM)-srclen/sizeof(MENUITEM)-1)
    return;
  for (j=0;j<srclen/sizeof(MENUITEM);j++)
    memcpy(&dest[i+j], &src[j], sizeof(MENUITEM));
  dest[i+j].name = NULL;
}

static void set_defaults(MENUITEM *item) {
  int i=0;
  while (item[i].name!=NULL) {
    if ((void (*)(int))item[i].callback == (void (*)(int))listbox_toggle)
      ((LISTBOX*)(item[i].callbackparam2))->callback(-1, &((LISTBOX*)(item[i].callbackparam2))->selected);
    if ((void (*)(int))item[i].callback == (void (*)(int))checkbox_toggle && item[i].data != NULL)
      ((void (*)(int, int*))item[i].data)(-1, item[i].callbackparam2);
    i++;
  } 
}

void set_machine_menu(int machine) {
  switch (machine) {
    case MENU_C64:
      add_menuitem(mainmenu, sizeof(mainmenu), c64menu, sizeof(c64menu));
      set_defaults(sidmenu);
      break;
  }
  add_menuitem(mainmenu, sizeof(mainmenu), mainmenu2, sizeof(mainmenu2));
  set_defaults(videomenu);
}
