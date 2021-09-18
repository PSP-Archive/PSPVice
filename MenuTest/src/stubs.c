#include "stdio.h"
#include "string.h"
#include "gl/glut.h"
#include "fileexp.h"
#include "unzip.h"
#include "Windows.h"
#include <direct.h>
#include "PngImage.h"

int PSPsmoothpixel;
int PSPArrayVideoMode_initialised;
int PSPVideoMode;
int PSPViceVideoMode;
unsigned int TextureFontInitialised;
int PSPFontNumber;

int disp_w=480;
int disp_h=272;

char path_root[2048]; // Store name of current dir (passed to main as argument). Should be (ms0:/PSP/Game/PSPVice)
char path_games[2048]; // Store games directories 

unsigned int paddata = 0; // Pressed
unsigned int new_pad = 0; // Just pressed
unsigned int new_rel = 0; // Just released
unsigned int old_pad = 0; // Save previous state to compute the 2 above values 

int is_paused;
int swap_joysticks;
int audio_active;
struct video_canvas_s * stored_canvas;
char gAutofire;
struct PSPInputs_name_struct * PSPInputs_AllNames;

unsigned int clut_face[16] =
{
  0xFF000000,
  0xFFFCFEFD,
  0xFF241ABE,
  0xFFC6E630,
  0xFFE21AB4,
  0xFF1ED21F,
  0xFFAE1B21,
  0xFF0AF6DF,
  0xFF0441B8,
  0xFF04336A,
  0xFF574AFE,
  0xFF404542,
  0xFF6F7470,
  0xFF59FE59,
  0xFFFE535F,
  0xFFA2A7A4,
};

// Allocs
int totalalloc=0;

#undef malloc
#undef free

void* Mymalloc(int size)
{
  void* ptr;
  totalalloc+=size;
  ptr=malloc(size);
  size = ((int*)ptr)[-4];
  printf("Malloc %d bytes. Total allocs %d\n", size, totalalloc);
  return ptr;
}

void Myfree(void* pointer)
{
  int size;
  size = ((int*)pointer)[-4];

  totalalloc-=size;
  printf("Free %d bytes. Total allocs %d\n", size, totalalloc);
  free(pointer);
}


void PSPDisplayText(int x,  int y, unsigned int color, const char* text)
{
  int len, i;
  glColor3f(  ((color&0x00ff0000)>>16) / 255.0f, ((color&0x0000ff00)>>8) / 255.0f, (color&0x000000ff) / 255.0f);
  len = (int) strlen(text);
  for (i = 0; i < len; i++) 
  {
    glRasterPos2f(x, y+8);
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, text[i]);
    x += 8;
  }
}

void PSPDisplayString(int x, int y, unsigned int color, char* string)
{
  PSPDisplayText(x,y,color,string);
}

void PSPSetFillColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A)
{
  glColor4f( R/255.0f, G/255.0f, B/255.0f, A/255.0f );
}

void PSPFillRectangle(unsigned int x, unsigned int y, unsigned int x2, unsigned int y2)
{
	 // Draw rectangle
	 glRectf(x,y,x2,y2);
}



void PSPDisplayImage(int x, int y, void* bitmap)
{
}

void PSPDisplayImage_Reset()
{
}

extern FILEENT files[8192];
extern int filesnb;

/*
// ----------------------------------------------------------------
// Name : SearchAndRecordExtraInfo
// 0=record a screenshot info
// 1=record a text info
// ----------------------------------------------------------------
void SearchAndRecordExtraInfo(char* fullnamewithpath, char* name, int mode)
{
  char shortname[512];
  char shortnamelist[512];
  char* pTmp;
  int i;
  
  // Extract name without extension
  strcpy(shortname, name);
  pTmp=strrchr(shortname, '.');
  if (pTmp)
    pTmp[0]=0; // Cut string after '.' character
  
  // Parse all recorded files to find if a disc or tape is matching the name
  for (i=0; i<filesnb; i++)
  {
    extractname(shortnamelist, files[i].name, NULL); // Extract name, no extension
  
    if (stricmp(shortnamelist, shortname)==0 )
    {
      if (mode==0)
        strcpy(files[i].screenshotname, fullnamewithpath);
      else if (mode==1)      
        strcpy(files[i].textname, fullnamewithpath);
      return; // end
    }
  }
}
*/

/*
//extern void extractname(char* result, char* source, char* ext);
// ----------------------------------------------------------------
// Name : RecordFileEntry (and sort it)
// ----------------------------------------------------------------
int RecordFileEntry(char* name)
{
  int result;
  int count;

  // Compare name to already inserted name
  // If find a name that value is high than current name, then we insert it there and shift all others names
  char currentshortname[512];

  extractname(currentshortname,name,NULL);
  
  result = -1;
  
  // Compare to existing names
  count=0;
  while (count!=filesnb && result==-1)
  {
    if ( strcmp( currentshortname, files[count].shortname ) < 0 )
    {
      result=count; // Insert place is found
    }
    if (result==-1)
      count++;
  }

  // If nothing found, add name at the end
  if (result==-1)
  {
    strcpy(files[filesnb].name, name);
    strcpy(files[filesnb].shortname,currentshortname);
    files[filesnb].flags = 0;
    files[filesnb].screenshotname[0]=0;
    files[filesnb].textname[0]=0;
    filesnb++;
    //printf("Record %s at place %d (add end) total filesnb %d", name,filesnb-1,filesnb);
    return filesnb-1;
  }
  else
  {
    // Shift all names from "result" to "filesnb"
    // And insert here our name
    // Copy name to next place. Start from end. Stop at "result" place
    int i;
    for (i=filesnb-1; i>=result; i--)
    {
      memcpy(&files[i+1],&files[i],sizeof(FILEENT));
    }
    strcpy(files[result].name, name);
    strcpy(files[result].shortname,currentshortname);
    files[result].flags = 0;
    files[result].screenshotname[0]=0;
    files[result].textname[0]=0;
    filesnb++;
    //printf("Record %s at place %d (inserted) total filesnb %d", name,result,filesnb);
    return result;
  }
}
*/

/*
// ----------------------------------------------------------------
// Name : 
// ----------------------------------------------------------------
int RecordDirEntry(char* name)
{
  char currentshortname[512];
  extractname(currentshortname,name,NULL);
  strcpy(files[filesnb].name, name);
  strcpy(files[filesnb].shortname,currentshortname);
  files[filesnb].flags = 1; // DIR
  files[filesnb].screenshotname[0]=0;
  files[filesnb].textname[0]=0;
  filesnb++;
  return filesnb-1;
}
*/

// ----------------------------------------------------------------
// 1 = Supported Image
// 2 = Zip files
// 3 = Screenshot files
// 4 = Text info files
// 5 = Text info files "nfo" (gamebase format)
// -1 = Unknown files type
/*
  * X64 (preferred) or D64 disk image filesnb; Used by the 1541, 2031, 3040, 4040 drives. 
  * G64 GCR-encoded 1541 disk image filesnb; 
  * D67 CBM2040 (DOS1) disk image format 
  * D71 VC1571 disk image format 
  * D81 VC1581 disk image format 
  * D80 CBM8050 disk image format 
  * D82 CBM8250/1001 disk image format 
  * T64 tape image filesnb (read-only); 
  * P00 program filesnb; 
*/
// ----------------------------------------------------------------
/*
int IsFileTypeRecognized(char* name)
{
  char* ext;
  ext=strrchr(name,'.');
      
  if (ext==NULL)
    return -1;

	if (    ( stricmp( ext, ".d64" ) == 0 )
	     || ( stricmp( ext, ".t64" ) == 0 )
	     || ( stricmp( ext, ".prg" ) == 0 )
	     || ( stricmp( ext, ".tap" ) == 0 )
	     || ( stricmp( ext, ".vsf" ) == 0 )
	     || ( stricmp( ext, ".g64" ) == 0 ) // DMC Added line
	     || ( stricmp( ext, ".p00" ) == 0 ) // Not supported yet
	   )
	{
    return 1;
  }
	else if ( stricmp( ext, ".zip" ) == 0 )
	{
    return 2;
  }
	else if ( stricmp( ext, ".png" ) == 0 )
	{
    return 3;
  }
	else if ( stricmp( ext, ".txt" ) == 0 )
	{
    return 4;
  }
	else if ( stricmp( ext, ".nfo" ) == 0 )
	{
    return 5;
  }
  else
  {
    return -1;
  }
}
*/
/*
// ----------------------------------------------------------------
// ParseZipFile
// mode : 0=Parse disk or tape  1=parse extra infos (png, txt)
// ----------------------------------------------------------------
void ParseZipFile(char* zipname, int mode)
{
  unzFile fd;
  unz_file_info info;
  int ret = 0;
  int filetype=-1;
  char fullfilename[512];
  char name[512];
  int nfofound; // Support GameBase nfo files (one Version.nfo files per archive)
  int filesindex[4096]; // Keep the place where filesnb are inserted
  int nbrecorderfiles;
  int i;
  
  // WIN32
  _chdir(path_games);

  // Attempt to open the archive
  fd = unzOpen(zipname);
  if(!fd)
  {
   printf("Can not open Zip files\n");
   return;
  }

  nfofound=0;
  nbrecorderfiles=0;

  // Go to first files in archive
  ret = unzGoToFirstFile(fd);
  if(ret != UNZ_OK)
  {
   printf("Can not go to first zip files\n");
   return;
  }

  do
  {
    ret = unzGetCurrentFileInfo(fd, &info, name, sizeof(name), NULL, 0, NULL, 0);
    if(ret != UNZ_OK)
    {
     printf("Can not read files info\n");
     return;
    }
    
    printf("Filename read from zip : %s\n", name);
    
    if (name)
      filetype = IsFileTypeRecognized(name);
    
	  strcpy(fullfilename,"z:");
	  strcat(fullfilename,zipname);
	  strcat(fullfilename,"/");
	  strcat(fullfilename,name);

    if ( mode == 0 ) // Parse disk or tape images
    {
  		if ( filetype == 1 )
  		{
  		  filesindex[nbrecorderfiles] = RecordFileEntry( fullfilename ); // Entries are sorted
  		  nbrecorderfiles++;
  		}
  		if ( filetype == 5 ) // the archive contain a nfo files
  		{
  		  if ( strcmp(name,"VERSION.NFO") == 0 )
  		  {
  		    nfofound=1; // One nfo for all filesnb in archive
  		  }
  		}
    }
    else if (mode == 1) // Parse extra infos
    {
  		if ( filetype == 3 ) // PNG
  		{
  		  SearchAndRecordExtraInfo(fullfilename, name, 0); // 0=record a screenshot info
      }    
  		if ( filetype == 4 ) // TXT
  		{
  		  SearchAndRecordExtraInfo(fullfilename, name, 1); // 1=record a text info
      }    
    }

  }
  while ( unzGoToNextFile(fd) == UNZ_OK );

  unzClose(fd);
  
  // -- Test if an nfo was found
  if ( (mode==0) && nfofound )
  {
	  strcpy(fullfilename,"z:"); // If yes, all filesnb in zip will received that text info filesnb (gamebase = 1zip for one game)
	  strcat(fullfilename,zipname);
	  strcat(fullfilename,"/Version.nfo");
    for (i=0; i<nbrecorderfiles; i++)
    {
      strcpy(files[filesindex[i]].textname,fullfilename);
    }
    //printf("nfo files added for files %d to %d (name %s)\n", filesnb-nbrecorderfiles, filesnb-1, fullfilename);
  }
}
*/

/*
void PSPFillFileArray()
{
  //char ext[4];
  char* pTmp;
  
  HANDLE            hFile;
	WIN32_FIND_DATA   FindData;
  BOOL              bFindOK;
  char              searchname[2048];

  //SceIoDirent de;
	//SceUID fd;

	int    filetype;

  // -- First pass, search disc or tape filesnb (or any recognized files format by VICE). Also search for palette filesnb vpl
  //memset( &de, 0, sizeof(SceIoDirent));

  char fullname[2048];  
  if ( PSPcheck_system_directory == 0 )	
  {
	  //fd = sceIoDopen( path_games );
    strcpy(searchname, path_games);
    strcat(searchname, "/*");
    hFile   =   FindFirstFile(searchname,   &FindData);
    RecordDirEntry(".."); // Add "up" directory
	}
	else
	{
    strcpy(fullname, path_root);
    strcat(fullname, "/PSPViceDatas/c64sys/");
    strcat(fullname, "*.*");
    hFile   =   FindFirstFile(searchname,   &FindData);   
	  //fd = sceIoDopen( fullname ) ;
	}
	
	if (1) // fd >= 0 )
	{
    bFindOK = (INVALID_HANDLE_VALUE   !=   hFile);
  	//while ( sceIoDread( fd, &de ) > 0 )
  	while ( bFindOK )
  	{
  	  //if (!strcmp(de.d_name, ".."))
  	  if (!strcmp((const char*)FindData.cFileName, ".."))
      {
        bFindOK   =   FindNextFile(hFile,   &FindData);
  	    continue;
      }
  	
  	  //if (!strcmp(de.d_name, "."))
  	  if (!strcmp((const char*)FindData.cFileName, "."))
      {
        bFindOK   =   FindNextFile(hFile,   &FindData);
  	    continue;
      }
  	  
  	  //if ( de.d_stat.st_mode == DIRECTORY )
      if ( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
  	  {
        //RecordDirEntry( de.d_name );
        RecordDirEntry( (char*) FindData.cFileName );
        bFindOK   =   FindNextFile(hFile,   &FindData);
        continue;
  	  }
  	  
      if ( PSPcheck_system_directory == 0 )
      {
        // Normal case, search for any supported type of files
      		//filetype = IsFileTypeRecognized(de.d_name);
      		filetype = IsFileTypeRecognized((char*)FindData.cFileName);
      		
      		if ( filetype == 1 )
      		{
      		  //RecordFileEntry( de.d_name );
      		  RecordFileEntry( (char*)FindData.cFileName );
      		}
      		else if ( filetype == 2 )
      		{
      		  // files is a zip archive, parse all filesnb inside
      		  // Record name of files with z:zipname.zip/filename
            //ParseZipFile( de.d_name, 0 );
            ParseZipFile( (char*)FindData.cFileName, 0 );
      		}
      }
      else
      {
        // Special Case for Palette
        // Not allowed to load printer palette "mps803" (make a crash)
        //pTmp=strrchr(de.d_name,'.');
        pTmp=(char*)strrchr((const char*)FindData.cFileName,'.');
        if (pTmp)
        {
      		if ( stricmp( pTmp, ".vpl" ) == 0 )
      		{
      		  //if ( stricmp( de.d_name, "mps803.vpl") != 0 ) // Not this name
      		  //  RecordFileEntry( de.d_name );
      		  if ( stricmp( (const char*)FindData.cFileName, "mps803.vpl") != 0 ) // Not this name
      		    RecordFileEntry( (char*)FindData.cFileName );
      		}
    		}
      }  		
      bFindOK   =   FindNextFile(hFile,   &FindData);
    } // While
  	//sceIoDclose( fd ) ; 
  }
  
  if ( PSPcheck_system_directory != 0 )
    return;
    
  // -- Second Pass ... Search for screenshots or text infos
  // Reparse all filesnb  
  //memset( &de, 0, sizeof(SceIoDirent));
  //strcpy(fullname, path_games);
  //fd = sceIoDopen( fullname ) ;

  strcpy(searchname, path_games);
  strcat(searchname, "/*.*");
  hFile   =   FindFirstFile(searchname,   &FindData);   
  
  if (1) // fd >= 0 )
	{
  	//while ( sceIoDread( fd, &de ) > 0 )
    bFindOK = (INVALID_HANDLE_VALUE   !=   hFile);
  	//while ( sceIoDread( fd, &de ) > 0 )
  	while ( bFindOK )
  	{
  	  //if (!strcmp(de.d_name, ".."))
  	  if (!strcmp((char*)FindData.cFileName, ".."))
      {
        bFindOK   =   FindNextFile(hFile,   &FindData);
  	    continue;
      }
  	
  	  //if (!strcmp(de.d_name, "."))
  	  if (!strcmp((char*)FindData.cFileName, "."))
      {
        bFindOK   =   FindNextFile(hFile,   &FindData);
  	    continue;
      }
  	    
   		//filetype = IsFileTypeRecognized(de.d_name);
   		filetype = IsFileTypeRecognized((char*)FindData.cFileName);
    		
  		if ( filetype == 3 ) // PNG
  		{
  		  //SearchAndRecordExtraInfo(de.d_name, de.d_name, 0); // 0=record a screenshot info
  		  SearchAndRecordExtraInfo((char*)FindData.cFileName, (char*)FindData.cFileName, 0); // 0=record a screenshot info
  		}
  		else if ( filetype == 4 ) // TXT
  		{
  		  //SearchAndRecordExtraInfo(de.d_name, de.d_name, 1); // 1=record a text info
  		  SearchAndRecordExtraInfo((char*)FindData.cFileName, (char*)FindData.cFileName, 1); // 1=record a text info
      }
  		else if ( filetype == 2 )
  		{
  		  // files is a zip archive, parse all files inside
  		  // Record name of files with z:zipname.zip/filename
        //ParseZipFile( de.d_name, 1 );
        ParseZipFile( (char*)FindData.cFileName, 1 );
  		}
      bFindOK   =   FindNextFile(hFile,   &FindData);  
  	} // while
  	//sceIoDclose( fd ) ; 
  } // if
}
*/

void PSPStartDisplay()
{
}

void PSPDisplayVideoBuffer()
{
}

void PSPWaitVBL()
{
}

void PSPSwap()
{
}

void PSPEndDisplay()
{
}

int  PSPGetVBLCount()
{
  return 0;
}


// Screenshot loaded name
char PSPCurrentScreenshotLoaded[512]={"\0"};

// Screenshot buffer (160*100 in 32bits or 320*200 for G64Base)
unsigned int PSPScreenshot[512*256]; // Store pixels of 160x100 or 320x200 pictures. OpenGl must be mapped then with power of 2 size
int          PSPScreenshotSizex;
int          PSPScreenshotSizey;

// OpenGl
GLuint opengl_textureid = -1;

// ----------------------------------------------------------------
// Name :  PSPDisplayScreenshot
// Display and load a screenshot
// NBchar. Number of caracters displaid before screenshots
// 10 = Name is cut. There is space for display info text
// 29 = No info text, name came near screenshot
// 40 = Names are displayd on screenshot place.
// ----------------------------------------------------------------
int PSPDisplayScreenshot( char* filename, int nbchar )
{
  int result;
  int i;
  int SCREENSHOTSTARTX;
  float u,v;
  int sizex;
  int sizey;
  
  glEnable(GL_TEXTURE_2D);

  // Test if screenshot need to be loaded
  if ( strcmp( PSPCurrentScreenshotLoaded, filename) != 0 ) // If screenshot is not loaded
  {
    // Unregister OpenGl Texture if needed
    if ( opengl_textureid != -1 )
    {
      glDeleteTextures(1,&opengl_textureid);
      opengl_textureid = -1;
    }

    result = PngLoadImage(filename, PSPScreenshot, &PSPScreenshotSizex, &PSPScreenshotSizey);
    if (result==1)
    {
      strcpy(PSPCurrentScreenshotLoaded, filename);
      printf("Image loaded %s\n", filename);
    
      // Register texture to opengl
      glGenTextures (1, &opengl_textureid);
      glBindTexture (GL_TEXTURE_2D, opengl_textureid);

      // Font use mipmap so we also need to use it there, only one filter mode allowed (Strange behaviour here !)
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      
      if ( PSPScreenshotSizex == 160 )
        glTexImage2D(GL_TEXTURE_2D,0,4,256,128,0,GL_RGBA,GL_UNSIGNED_BYTE,PSPScreenshot);
      else
        glTexImage2D(GL_TEXTURE_2D,0,4,512,256,0,GL_RGBA,GL_UNSIGNED_BYTE,PSPScreenshot);

    }
    else
    {
      PSPCurrentScreenshotLoaded[0]=0;
      return -1; // error loading picture
    }
  }

  // Display image
  // Define start X of image ... 3*8*12 is size of external bordel, 4 is size of internal border
  SCREENSHOTSTARTX = (480 - (3*12) - 2 - 160)+1;
  if ( nbchar > 29 )
   SCREENSHOTSTARTX += (nbchar-29)*8;
  #define SCREENSHOTSTARTY ((4*12) + 2)

  //glColor4f(1.0f,1.0f,1.0f,1.0f);

  // Display Screenshot
  if ( PSPScreenshotSizex == 160 ) // 160*100
  {
    u = 160.0f/256.0f;
    v = 100.0f/128.0f;
  }
  else // 320*200
  {
    u = 320.0f/512.0f;
    v = 200.0f/256.0f;
  }

  glBegin(GL_QUADS);
  glTexCoord2f(0.0f,0.0f); 
  glVertex2f(SCREENSHOTSTARTX, SCREENSHOTSTARTY);

  glTexCoord2f(u,0.0f); 
  glVertex2f(SCREENSHOTSTARTX+160, SCREENSHOTSTARTY);

  glTexCoord2f(u,v); 
  glVertex2f(SCREENSHOTSTARTX+160, SCREENSHOTSTARTY+100);

  glTexCoord2f(0.0f,v); 
  glVertex2f(SCREENSHOTSTARTX, SCREENSHOTSTARTY+100);
  glEnd();

  glDisable(GL_TEXTURE_2D);

  return 0;
}

void PSPGetDateAndTime(char* buffer)
{
}

int file_system_attach_disk(unsigned int a,char const *b)
{
  return 0;
}
int resources_set_value(char const * a,void * b)
{
  return 0;
}
int autostart_autodetect(char const * a,char const * b,unsigned int c,unsigned int d)
{
  return 0;
}
void file_system_detach_disk(int a)
{
}
int resources_get_value(char const * a,void * b)
{
  *(int*)b=1;  // Video standard
  return 0;
}
void PrefSetVideoMode(unsigned char a)
{
}
void PrefSetVideoPSPDisplayMode(unsigned char a)
{
}
unsigned char PrefGetVideoPSPDisplayMode(void)
{
  return 1;
}
void PrefSetFontMenu(unsigned char a)
{
}
unsigned char PrefGetFontMenu(void)
{
  return 10;
}
void PrefSetPSPCpuSpeed(unsigned char a)
{
}
void PSPChangeCpuFrequency(int a)
{
}
unsigned char PrefGetPSPCpuSpeed(void)
{
  return 222;
}
int machine_write_snapshot(char const * a,int b,int c,int d)
{
  return 0;
}
unsigned char * ps2_videobuf;


void interrupt_maincpu_trigger_trap(void* a,void * c)
{
}

int machine_read_snapshot(char const * a,int b)
{
  return 0;
}
void vkeyboard_activate(void)
{
}
void machine_trigger_reset(unsigned int a)
{
}
void joystick_clear_all(void)
{
}
void pad_update(void)
{
}

void PSPSetPaletteEntry(int id, unsigned char red,unsigned char green, unsigned char blue)
{
}

void pspaudio_init(void)
{
}

void pspaudio_clear(void)
{
}

void pspaudio_write(short* buffer, int num_samples)
{
}

void pspaudio_shutdown(void)
{
}

void pspaudio_SetEffect(int value)
{
}

void          PSPInputs_Init()
{
}

void          PSPInputs_Update()
{
}

unsigned char PSPInputs_IsButtonPressed(unsigned char value)
{
    return 0;
}

unsigned char PSPInputs_IsButtonJustPressed(unsigned char value)
{
    return 0;
}

unsigned char PSPInputs_IsButtonJustReleased(unsigned char value)
{
    return 0;
}

unsigned char PSPInputs_GetMappingValue(unsigned char pspkey)
{
    return 0;
}

void          PSPInputs_SetMappingValue(unsigned char pspkey, unsigned char value)
{
}

void          PSPInputs_Set2playersKeyMapping()
{
}

void          PSPInputs_RestoreKeyMapping()
{
}

unsigned char PSPInputs_GetNameIndexFromFunctionValue(unsigned char value)
{
    return 0;
}

void          PSPInputs_Load(const char* number)
{
}

void          PSPInputs_Save(const char* number)
{
}

void sceKernelExitGame(void)
{
}

void SavePreferences(void)
{
}

void kbdbuf_feed(char* name)
{
}

void archdep_close_default_log_file(void)
{
}

void sceGuTexFlush()
{
}
void sceKernelDcacheWritebackAll()
{
}
void sceDisplayWaitVblankStart()
{
}
void sceDisplayGetFrameBuf(void** a, int* b, int* c, int* d)
{
}
