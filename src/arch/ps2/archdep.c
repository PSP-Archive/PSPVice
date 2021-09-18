/*
 * archdep.c - Miscellaneous system-specific stuff.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <viceteam@t-online.de>
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#include "vice.h"

#include <tamtypes.h>
#include <eefileio.h>
#include <malloc.h>
#include <kernel.h>
#include <compat.h>
#include <sifcmd.h>
#include <sifrpc.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

#include "archdep.h"
#include "fcntl.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "util.h"
#include "video.h"
#include "ps2main.h"
#include "support.h"
#include "cdvd_vsync.h"

char mtolower(char c) {
  if (c>='A' && c<='Z')
    return c-('A'-'a');
  return c;
}

static void restore_workdir(void)
{
    printf ("restore_workdir\n");
}

int archdep_init(int *argc, char **argv)
{
    printf ("archdep_init\n");
    return 0;
}

char *archdep_program_name(void)
{
    return lib_stralloc("xvic");
}

const char *archdep_boot_path(void)
{
    return lib_stralloc("host:");
}

char *archdep_default_sysfile_pathlist(const char *emu_id)
{
    char tmp[1024];
    char *p;

    snprintf (tmp, sizeof(tmp), "%ssys/", emu_id);

    p = tmp;
    while(*p!='\0') {
      *p = mtolower(*p);
      p++;
    }
    return lib_stralloc(tmp);
}

/* Return a malloc'ed backup file name for file `fname'.  */
char *archdep_make_backup_filename(const char *fname)
{
    printf ("archdep_make_backup_filename\n");
    return lib_stralloc("");
}

char *archdep_default_save_resource_file_name(void)
{
    return archdep_default_resource_file_name();
}

char *archdep_default_resource_file_name(void)
{
    return util_concat(archdep_boot_path(), "vicerc", NULL);
}

char *archdep_default_fliplist_file_name(void)
{
    return lib_stralloc("");
}

FILE *archdep_open_default_log_file(void)
{
    return NULL;
}

int archdep_num_text_lines(void)
{
    return 25;
}

int archdep_num_text_columns(void)
{
    return 80;
}

int archdep_default_logger(const char *level_string, const char *txt)
{
    return 0;
}

void archdep_setup_signals(int do_core_dumps)
{
}

int archdep_path_is_relative(const char *path)
{
    if (path == NULL)
        return 0;

    /* `c:\foo', `c:/foo', `c:foo', `\foo' and `/foo' are absolute.  */

/*
    if (index(path, ':')!=NULL)
      return 0;
*/
    return !(path[0] == '/' || path[0] == '\\');
}

int archdep_spawn(const char *name, char **argv,
                  const char *stdout_redir, const char *stderr_redir)
{
    return 1;
}

/* return malloc´d version of full pathname of orig_name */
int archdep_expand_path(char **return_path, const char *orig_name)
{
    *return_path = lib_stralloc("");
    return 0;
}

void archdep_startup_log_error(const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    vfprintf(stderr, format, ap);
}

char *archdep_filename_parameter(const char *name)
{
    /* nothing special(?) */
    return lib_stralloc(name);
}

char *archdep_quote_parameter(const char *name)
{
    /*not needed(?) */
    return lib_stralloc(name);
}

char *archdep_tmpnam(void)
{
    return "foobar";
}

int archdep_file_is_gzip(const char *name)
{
/*
    size_t l = strlen(name);

    if ((l < 4 || strcasecmp(name + l - 3, ".gz"))
        && (l < 3 || strcasecmp(name + l - 2, ".z"))
        && (l < 4 || toupper(name[l - 1]) != 'Z' || name[l - 4] != '.'))
        return 0;
    return 1;
*/
    return 0;
}

int archdep_file_set_gzip(const char *name)
{
  return 0;
}

int archdep_mkdir(const char *pathname, int mode)
{
    return 0;
}

int archdep_stat(const char *file_name, unsigned int *len, unsigned int *isdir)
{
    return 0;
}

int archdep_file_is_blockdev(const char *name)
{
    return 0;
}

int archdep_file_is_chardev(const char *name)
{
    return 0;
}

void archdep_shutdown(void)
{
}

extern int ps2_framecount;

time_t time(time_t *tim) {
  return ps2_framecount / 50;
}

extern FILE *fopen_(const char *c, const char *d, int test);

char mtoupper(char c) {
  if (c>='a' && c<='z')
    return c+'A'-'a';
  return c;
}


int file_exist(const char *name) {
  int i=0;
  char fn[128];

  char upper[128];
  char upper2[128];
  char lower[128];
  char allupper[128];
  char alllower[128];

//  printf ("testing '%s'\n", name);

  snprintf (upper, sizeof(upper), "%s", name);
  i = strlen(upper);
  while (i>=0) {
    if (lower[i]=='/' || lower[i]==':')
      break;
    upper[i] = mtoupper(upper[i]);
    i--;
  }
  snprintf (upper2, sizeof(upper2), "%s", name);
  i = strlen(upper2);
  while (i>=0) {
    if (upper2[i]==':')
      break;
    upper2[i] = mtoupper(upper2[i]);
    i--;
  }
  snprintf (lower, sizeof(lower), "%s", name);
  i = strlen(lower);
  while (i>=0) {
    if (lower[i]=='/' || lower[i]==':')
      break;
    lower[i] = mtolower(lower[i]);
    i--;
  }
  snprintf (alllower, sizeof(alllower), "%s", name);
  i = strlen(alllower);
  while (i>=0) {
    alllower[i] = mtolower(alllower[i]);
    i--;
  }
  snprintf (allupper, sizeof(allupper), "%s", name);
  i = strlen(allupper);
  while (i>=0) {
    allupper[i] = mtoupper(allupper[i]);
    i--;
  }

  snprintf (fn, sizeof(fn), "%s", name);
  if ((fopen_(fn, "r", 1))!=NULL) {
    return 1;
  }
  snprintf (fn, sizeof(fn), "%s%s", path_prefix, name);
  if ((fopen_(fn, "r", 1))!=NULL) {
    return 1;
  }
  snprintf (fn, sizeof(fn), "%s%s", path_prefix, allupper);
  if ((fopen_(fn, "r", 1))!=NULL) {
    return 1;
  }
  snprintf (fn, sizeof(fn), "%s", upper);
  if ((fopen_(fn, "r", 1))!=NULL) {
    return 1;
  }
  snprintf (fn, sizeof(fn), "%s", upper2);
  if ((fopen_(fn, "r", 1))!=NULL) {
    return 1;
  }
  snprintf (fn, sizeof(fn), "%s;1", upper);
  if ((fopen_(fn, "r", 1))!=NULL) {
    return 1;
  }
  snprintf (fn, sizeof(fn), "%s;1", upper2);
  if ((fopen_(fn, "r", 1))!=NULL) {
    return 1;
  }
  snprintf (fn, sizeof(fn), "%s", lower);
  if ((fopen_(fn, "r", 1))!=NULL) {
    return 1;
  }
  snprintf (fn, sizeof(fn), "%s%s;1", path_prefix, allupper);
  if ((fopen_(fn, "r", 1))!=NULL) {
    return 1;
  }
  snprintf (fn, sizeof(fn), "%s%s", path_prefix, alllower);
  if ((fopen_(fn, "r", 1))!=NULL) {
    return 1;
  }
  return 0;
}

FILE *tempfile[4] = {NULL, NULL, NULL, NULL};

FILE *fopen(const char *name, const char *mode) {
  int i=0;
  FILE *f;
  char fn[128];
  char upper[128];
  char upper2[128];
  char lower[128];
  char allupper[128];
  char alllower[128];

  snprintf (upper, sizeof(upper), "%s", name);
  i = strlen(upper);
  while (i>=0) {
    if (upper[i]=='/' || upper[i]==':')
      break;
    upper[i] = mtoupper(upper[i]);
    i--;
  }
  snprintf (upper2, sizeof(upper2), "%s", name);
  i = strlen(upper2);
  while (i>=0) {
    if (upper2[i]==':')
      break;
    upper2[i] = mtoupper(upper2[i]);
    i--;
  }
  snprintf (lower, sizeof(lower), "%s", name);
  i = strlen(lower);
  while (i>=0) {
    if (lower[i]=='/' || lower[i]==':')
      break;
    lower[i] = mtolower(lower[i]);
    i--;
  }
  snprintf (alllower, sizeof(alllower), "%s", name);
  i = strlen(alllower);
  while (i>=0) {
    alllower[i] = mtolower(alllower[i]);
    i--;
  }
  snprintf (allupper, sizeof(allupper), "%s", name);
  i = strlen(allupper);
  while (i>=0) {
    allupper[i] = mtoupper(allupper[i]);
    i--;
  }

  snprintf (fn, sizeof(fn), "%s", name);
  if ((f=fopen_(fn, mode, 0))!=NULL)
    return f;
  if (!strncmp(fn, "temp", 4))
    return 0;
  snprintf (fn, sizeof(fn), "%s%s", path_prefix, name);
  if ((f=fopen_(fn, mode, 0))!=NULL)
    return f;
  snprintf (fn, sizeof(fn), "%s%s", path_prefix, allupper);
  if ((f=fopen_(fn, mode, 0))!=NULL)
    return f;
  snprintf (fn, sizeof(fn), "%s", upper);
  if ((f=fopen_(fn, mode, 0))!=NULL)
    return f;
  snprintf (fn, sizeof(fn), "%s", upper2);
  if ((f=fopen_(fn, mode, 0))!=NULL)
    return f;
  snprintf (fn, sizeof(fn), "%s;1", upper);
  if ((f=fopen_(fn, mode, 0))!=NULL)
    return f;
  snprintf (fn, sizeof(fn), "%s;1", upper2);
  if ((f=fopen_(fn, mode, 0))!=NULL)
    return f;
  snprintf (fn, sizeof(fn), "%s", lower);
  if ((f=fopen_(fn, mode, 0))!=NULL)
    return f;
  snprintf (fn, sizeof(fn), "%s%s;1", path_prefix, allupper);
  if ((f=fopen_(fn, mode, 0))!=NULL)
    return f;
  snprintf (fn, sizeof(fn), "%s%s", path_prefix, alllower);
  if ((f=fopen_(fn, mode, 0))!=NULL)
    return f;

  return f;
}

int numwr = 0;


int fclose(FILE *f) {
  numwr = 0;
//  printf ("close %d (%s)%s\n", f->_file, f->_flags&1 ? "w" : "r", f->_ur!=-1 ? " (tempfile)" : "");
  if (f->_ur!=-1) {
    return 0;
  }
  if (f->_flags & 1) {
    fioLseek(f->_file, 0, SEEK_SET);
    fioWrite(f->_file, f->_p, f->_r);
    fio_close(f->_file);
  }
  free(f->_p);
  free(f);
  return 0;
}

long ftell(FILE *f) {
  return f->_offset;
}

void rewind(FILE *f) {
  f->_offset = 0;
}

int fseek(FILE *f, long offset, int whence) {
  if (whence == SEEK_END)
    f->_offset = f->_r + offset;
  else if (whence == SEEK_CUR)
    f->_offset += offset;
  else if (whence == SEEK_SET)
    f->_offset = offset;

  if (f->_offset < 0)
    f->_offset = 0;
  if (f->_offset > f->_r)
    f->_offset = f->_r;

//  printf ("seek: offset = %d  r = %d\n", f->_offset, f->_r);

  if (f->_offset < 0 || f->_offset > f->_r) {
    f->_offset = f->_r;
    return -1;
  }

  return 0;
}

char *fgets(char *s, int size, FILE *f) {
  int p=0, c;
  while (f->_offset < f->_r) {
    c = f->_p[f->_offset++];
    if (p<size-1) {
      s[p++] = c;
    }
    if (c=='\n') {
      break;
    }
  }
  s[p]='\0';
  if (f->_offset >= f->_r && p==0)
    return NULL;
  return s;
}

int feof_(FILE *f) {
  if (f->_offset >= f->_r)
    return 1;
  return 0;
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *f) {
  int i;
  for (i=0;i<nmemb;i++) {
    if (f->_offset + size > f->_r)
      break;
    memcpy(ptr, &f->_p[f->_offset], size);
    ptr += size;
    f->_offset += size;
  }
  return i;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *f) {
  int len = size*nmemb, i;
//  printf ("fwrite %d %d (%d)\n", size, nmemb, len);  
  if (f->_offset+len > f->_r) {
    f->_p = realloc(f->_p, f->_offset+len);
    f->_r = f->_offset+len;
  }
  for (i=0;i<len;i++) {
    f->_p[f->_offset++] = ((unsigned char*)ptr)[i];
  }
  numwr += len;
//  printf ("numwr = %d  offset = %d  r = %d\n", numwr, f->_offset, f->_r);
  return nmemb;
}

int fputc(int c, FILE *f) {
  if (f == stdout) {
    printf ("%c", c);
    return (unsigned char)c;
  }
  if (f->_offset >= f->_r) {
    f->_p = realloc(f->_p, f->_r+1);
    f->_r++;
  }
  f->_p[f->_offset++] = c;
//  printf ("fputc %d %d\n", c, f->_file);
//  printf ("numwr = %d  offset = %d  r = %d\n", ++numwr, f->_offset, f->_r);
  return (unsigned char)c;
}

int fputs(const char *s, FILE *f) {
  int len = strlen(s), i;
  if (f == stdout) {
    printf ("%s", s);
    return len;
  }
  if (f->_offset+len > f->_r) {
    f->_p = realloc(f->_p, f->_offset+len);
    f->_r = f->_offset+len;
  }
  for (i=0;i<len;i++) {
    f->_p[f->_offset++] = s[i];
  }
  return len;
}

int fgetc(FILE *f) {
  if (f->_offset >= f->_r)
    return -1;
  return f->_p[f->_offset++];
}
