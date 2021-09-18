#ifndef _INFOWIN_H
#define _INFOWIN_H

extern int infowin_active;
extern int aboutwin_active;

void infomsg_frame();
void show_info_msg(char *message, int timeout);
void aboutwin_set_text(const char *text, int h);
void show_infowin_nonblock(char *message);
void show_aboutwin_nonblock();
void infowin_frame();
void aboutwin_frame();

#endif

