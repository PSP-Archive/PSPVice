#ifndef _MENU_H
#define _MENU_H

enum {MENU_C64, MENU_C128, MENU_VIC20, MENU_PLUS4, MENU_CBM2, MENU_PET};

extern int menu_active;

int menu_update();
void set_machine_menu(int machine);

#endif
