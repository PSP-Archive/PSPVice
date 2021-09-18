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

//#define PSPDEBUG // display printf

#include "vice.h"

#include <malloc.h>
#include <signal.h>
#include <string.h>
#include "archdep.h"
#include "fcntl.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "util.h"
#include "video.h"
#include "pspmain.h"
#include "support.h"

#ifdef LIBMAL
#include "ps2fs.h"
#include "sifdev.h"
#endif

char mtolower(char c) {
  if (c>='A' && c<='Z')
    return c-('A'-'a');
  return c;
}

//DMC - added log file
FILE *defaultLogFile = NULL; 

static void restore_workdir(void)
{
    //printf ("restore_workdir\n");
}

int archdep_init(int *argc, char **argv)
{
    //printf ("archdep_init\n");
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
    //printf ("archdep_make_backup_filename\n");
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
//DMC added function
void archdep_close_default_log_file(void)
{
	if (defaultLogFile != NULL)
	{
		fclose(defaultLogFile);
		defaultLogFile = NULL;
	}
}
//DMC -end
//DMC - changed to actually open a log file
FILE *archdep_open_default_log_file(void)
{
	#ifdef PSPDEBUG
  printf("archdep_open_default_log_file\n");
  #endif
	FILE *f = fopen("logfile","w+");
	if (f != NULL)
	{
		defaultLogFile = f;
		printf("success\n");
		fputs(f,"Log opened...");
	}
	
    return f;

//return NULL;

}
//DMC -end

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
                         char **pstdout_redir, const char *stderr_redir)
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
    //vfprintf(stderr, format, ap);
    printf(format, ap);
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

/*
time_t time(time_t *tim)
{
  return ps2_framecount / 50;
}
*/

// ---------------------------------------------------------------
// Name : 
// ---------------------------------------------------------------
FILE *archdep_mkstemp_fd(char **filename, const char *mode)
{
    char *tmp;
    FILE *fd;

    tmp = lib_stralloc(tmpnam(NULL));

    fd = fopen(tmp, mode);

    if (fd == NULL)
        return NULL;

    *filename = tmp;

    return fd;
} 

