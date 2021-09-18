Changes made by Dennis Crews

Note that all changed lines are labeled with a comment with my initials. 
Grep for DMC to find them all except the Makefile changes.

I did not comment my changes to the Makefile!!!!!

New Features
--------------
True Drive Emulation  (Woo Hoo!!!)
Quit opion on main menu

Changed files
-------------------------
Makefile
CommonHeader.h
src/arch/psp/archdep.c
src/arch/psp/menu.c
src/arch/psp/PSPSpecific.c
src/drive/tcbm/tpid.c
src/log.c

New files
---------
lib directory (My PSP dev environment did not have lpng or zlib, so here they are for static compiling)
plus4 directory (only 	src\plus4\plus4tcbm.c is needed...this is for true drive emulation)


Summary of Makefile changes
---------------------------
added two new defines to CFLAGS:  -DHAVE_DIRENT -DPSP
added instructions to automatically generate the EBOOT.PBP file
added all lib files to OBJS
added src/plus4/plus4tcbm.o to OBJS

Summary of other file changes

CommonHeader.h
	Removed #define PSP, moved to Makefile
	
src/arch/psp/archdep.c
	enabled opening/closing default log file. archdep_open_default_log_file, archdep_close_default_log_file
	
src/arch/psp/menu.c
	added "Quit PSPVice" to the main menu, added quit_program function
	
src/arch/psp/PSPSpecific.c
	added .g64 extention to IsFileTypeRecognized as a valid extension
	
src/drive/tcbm/tpid.c
	removed all #ifndef PSP, re-enabling these functions. They control the main CPU communicating with the 1541 drive
	
src/log.c
	uncommented the log_helper calls to enable writing to the default log.
	