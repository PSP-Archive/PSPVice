#include <stdio.h>
#include <tamtypes.h>
#include <eefileio.h>
#include <string.h>
#include <cdvd_rpc.h>

#include "gs.h"
#include "g2.h"
#include "osdmsg.h"
#include "bitmap.h"
#include "libpad.h"
#include "pad.h"
#include "menu.h"
#include "infowin.h"
#include "support.h"
#include "cdvd_vsync.h"

#define SELIW 200
#define WINW 250
#define WINH 200
#define WINX ((disp_w-WINW)/2)
#define WINY ((disp_h-WINH)/2)
#define PERPAGE 13
#define SBYSTART (WINY+24)
#define SBYEND (WINY+WINH)
#define SBXSTART (WINX+WINW-8)
#define SBXEND (WINX+WINW)
#define SBH (SBYEND-SBYSTART)

int file_explorer_active = 0;
char file_explorer_dir[256]="";
static void (*callback)() = NULL;
static void *callbackparam = NULL;

extern FONT f_arial;

#define F_DIR 1

typedef struct {
  char name[256];
  unsigned char flags;
} FILEENT;

static FILEENT file[4096];
static int files = 0;
static int selected = 0;
static int scrollpos = 0;

static const char *drives[] = {"cdrom0:", "mc0:", "mc1:", "hdd:", "host:"};

static int readdir_std() {
  int r, s;
  fio_dirent_t ent;
  files = 0;
  if (!*file_explorer_dir) {
    for (r=0;r<sizeof(drives)/sizeof(char*);r++) {
      sprintf(file[r].name, "[%s]", drives[r]);
      file[r].flags = F_DIR;
      files++;
    }  
    return files;
  }
  show_msg("Reading directory..");
  r = fioDopen(file_explorer_dir);
  if (r < 1)
    return 0;
  strcpy(file[0].name, "[..]");
  file[0].flags = F_DIR;
  files = 1;
  while (1) {
    s = fioDread(r, &ent);
    if (s < 1)
      break;;
    if (!strcmp(ent.name, ".."))
      continue;
    if (!strcmp(ent.name, "."))
      continue;
    if (!*ent.name)
      continue;    

    if (ent.stat.mode & 16) {
      snprintf (file[files].name, sizeof(file[0].name), "%s", ent.name);
      file[files].flags = 0;
    } else {
      snprintf (file[files].name, sizeof(file[0].name), "[%s]", ent.name);
      file[files].flags = F_DIR;
    }
    files++;
  }
  fioDclose(r);
  return files;
}

static int readdir_cdvd(char *path) {
  int r, i;
  struct TocEntry toc[4096];
  files = 0;

  show_msg("Reading directory..");

  cdvd_access();

  CDVD_FlushCache();
  if (CDVD_DiskReady(CdNonBlock)!=CdComplete)
    return 0;
  r = CDVD_GetDir(path, NULL, CDVD_GET_FILES_AND_DIRS, toc, 4096, NULL);
  if (r < 1)
    return 0;
  strcpy(file[0].name, "[..]");
  file[0].flags = F_DIR;
  files = 1;
  for (i=0;i<r;i++) {
    if (!strcmp(toc[i].filename, ".."))
      continue;
    if (!strcmp(toc[i].filename, "."))
      continue;
    if (!*toc[i].filename)
      continue;    
    if (toc[i].fileProperties & 2) {
      snprintf(file[files].name, sizeof(file[0].name), "[%s]", toc[i].filename);
      file[files].flags = F_DIR;
    } else {
      snprintf(file[files].name, sizeof(file[0].name), "%s", toc[i].filename);
      file[files].flags = 0;
    }
    files++;
  }
  return files;
}

static int readdir() {
  int r;
  scrollpos = 0;
  selected = 0;
  if (!strncmp(file_explorer_dir, "cdrom0:", 7)) {
    r = readdir_cdvd(file_explorer_dir+7);
    if (r<1) {
      show_infowin_nonblock("CDVD not ready.");
    }
    return r;
  } else {
    return readdir_std();
  }
}

void launch_file_explorer(void (*callbac)(), void *callbackpara) {
  int r;
  file_explorer_active = 1;
  selected = 0;
  callback = callbac;
  callbackparam = callbackpara;
  r = readdir();  
  if (r<1) {
    strcpy(file_explorer_dir, "");
    readdir();
  }
}

static void draw_scrollbar() {
  int hei;
  int start;
  start = (scrollpos/(float)files)*SBH+0.5;
  hei = (PERPAGE/(float)files)*SBH+0.5;
  if (hei < SBH) {
    g2_set_fill_color(80, 80, 80);
    g2_fill_rect(SBXSTART, SBYSTART, SBXEND, SBYEND);
    g2_set_fill_color(180, 180, 180);
    g2_fill_rect(SBXSTART, SBYSTART+start, SBXEND, SBYSTART+hei+start);
    g2_set_fill_color(0, 0, 0);
  }
}

static void go_upper() {
  char drive[64];
  char path[256];
  char temp[256+64];
  char *p;
  snprintf(temp, sizeof(temp), "%s", file_explorer_dir);
  p = index(temp, ':');
  if (p!=NULL)
    *p='\0';
  else
    return;
  snprintf(drive, sizeof(drive), "%s:", temp);
  snprintf(path, sizeof(path), "%s", p+1);
  if (strlen(path) && path[strlen(path)-1]=='/')
    path[strlen(path)-1]='\0';
  if (!*path) {
    *file_explorer_dir='\0';
    return;    
  }
  p = rindex(path, '/');
  if (p == NULL) {
    snprintf (file_explorer_dir, sizeof(file_explorer_dir), "%s", drive);
    return;
  }
  *(p+1)='\0';
  snprintf (file_explorer_dir, sizeof(file_explorer_dir), "%s%s", drive, path);
}

static void enter_dir(char *name) {
  char buf[256+64];
  char *n;
  char nbuf[256+64];
  n = name;
  if (name[0]=='[' && name[strlen(name)-1]==']') {
    n = nbuf;
    snprintf(nbuf, sizeof(nbuf), "%s", name+1);
    nbuf[strlen(nbuf)-1]='\0';
  }
  if (!strcmp(n, "host:"))
    snprintf (buf, sizeof(buf), "%s%s", file_explorer_dir, n);
  else
    snprintf (buf, sizeof(buf), "%s%s/", file_explorer_dir, n);
  snprintf (file_explorer_dir, sizeof(file_explorer_dir), "%s", buf);
}

void file_explorer_frame() {
  int i, r, f;
  static int repeatcntr = 0;
  static int repeatspeed = 0;
  g2_set_fill_color(20, 20, 20);
  g2_fill_rect(WINX, WINY, WINX+WINW, WINY+WINH);
  render_text_c (&f_arial, WINX, WINY+1, WINW, file_explorer_dir, 0xF0F0F0);
  g2_set_fill_color(90, 90, 90);
  g2_fill_rect(WINX+WINW/2-SELIW/2, WINY+18+(selected-scrollpos)*13+6, WINX+WINW/2+SELIW/2, WINY+18+(selected-scrollpos)*13+12+6);
  g2_set_fill_color(0, 0, 0);

  for (i=0;i<PERPAGE;i++) {
    if (i+scrollpos >= files)
      break;
    if (i+scrollpos == selected) {
      render_text_c(&f_arial, WINX, WINY+18+i*13, WINW, file[i+scrollpos].name, 0xF0F0F0);
    } else {
      render_text_c(&f_arial, WINX, WINY+18+i*13, WINW, file[i+scrollpos].name, 0x30C070);
    }
  }

  if (new_rel & PAD_START) {
    file_explorer_active = 0;
    menu_active = 0;
  }
  if (new_pad & PAD_TRIANGLE) {
    if (!*file_explorer_dir)
      file_explorer_active = 0;
    else {
      go_upper();
      r = readdir();  
      if (r<1) {
        strcpy(file_explorer_dir, "");
        readdir();
      }
    }
  }
  if (new_pad & PAD_CROSS) {
    if (file[selected].flags & F_DIR) {
      if (!strcmp(file[selected].name, "[..]"))
        go_upper();
      else
        enter_dir(file[selected].name);
      r = readdir();  
      if (r<1) {
        strcpy(file_explorer_dir, "");
        readdir();
      }
    } else {
      char buf[512];
      if (!strncmp(file_explorer_dir, "cdrom0:", 7))
        snprintf (buf, sizeof(buf), "cdfs:%s%s", file_explorer_dir+7, file[selected].name);
      else
        snprintf (buf, sizeof(buf), "%s%s", file_explorer_dir, file[selected].name);
      callback(buf, callbackparam);
      file_explorer_active = 0;
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
    selected--;
    if (selected < 0)
      selected = files-1;
    if (selected < scrollpos)
      scrollpos = selected;
    if (selected >= scrollpos+PERPAGE)
      scrollpos = selected - PERPAGE + 1;
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
    selected++;
    if (selected >= files)
      selected = 0;
    if (selected < scrollpos)
      scrollpos = selected;
    if (selected >= scrollpos+PERPAGE)
      scrollpos = selected - PERPAGE + 1;
  }


  f = 0;
  if (new_pad & PAD_LEFT) {
    f = 1;
    repeatspeed=0;
  } else if (paddata & PAD_LEFT) {
    repeatcntr++;
    if ((repeatspeed>1&&repeatcntr>3)||repeatcntr>18)
      f = 1;
  }
  if (f) {
    repeatcntr = 0;
    repeatspeed++;
    selected-=PERPAGE;
    if (selected < 0)
      selected = 0;
    if (selected < scrollpos)
      scrollpos = selected;
    if (selected >= scrollpos+PERPAGE)
      scrollpos = selected - PERPAGE + 1;
  }

  f = 0;
  if (new_pad & PAD_RIGHT) {
    f = 1;
    repeatspeed=0;
  } else if (paddata & PAD_RIGHT) {
    repeatcntr++;
    if ((repeatspeed>1&&repeatcntr>3)||repeatcntr>18)
      f = 1;
  }
  if (f) {
    repeatcntr = 0;
    repeatspeed++;
    selected+=PERPAGE;
    if (selected >= files)
      selected = files - 1;
    if (selected < scrollpos)
      scrollpos = selected;
    if (selected >= scrollpos+PERPAGE)
      scrollpos = selected - PERPAGE + 1;
  }

  draw_scrollbar();
}
