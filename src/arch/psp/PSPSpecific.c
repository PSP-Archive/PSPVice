#ifndef WIN32
#include "pspiofilemgr.h"
#include <pspkernel.h>
#include <pspdisplay.h> 
#include "pspgu.h"
#include "pspgum.h"
#include "psprtc.h"
#else
#include "Windows.h"
#endif

#include "PSPSpecific.h"
#include "unzip.h"
#include "string.h"
#include "fileexp.h"
#include "PngImage.h"

#ifndef WIN32
extern unsigned int __attribute__((aligned(16))) list[262144];
#endif

extern unsigned char *ps2_videobuf;

extern char path_root[];
extern char path_games[];
extern int  gZipCurrentFileLocation;
extern int  gZipCurrentNumFile;

const int DIRECTORY = 4607;

#ifndef WIN32

#define SPRITE_SIZE_IN_FLOATS 12
#define SPRITE_SIZE_IN_BYTE (SPRITE_SIZE_IN_FLOATS*4)
#define MAX_SPRITE_RENDER 2040

#define QUAD_SIZE_IN_FLOATS 30
#define MAX_BITMAP_RENDER 20

// Quad, for render screen

static float PSPRectangle[MAX_BITMAP_RENDER][QUAD_SIZE_IN_FLOATS] __attribute__((aligned(16)));

static int   PSPCurrentNumberOfRenderedRectangle=0;

// Textured sprite for font

static float PSPSprites[MAX_SPRITE_RENDER][SPRITE_SIZE_IN_FLOATS] __attribute__((aligned(16)));

static int   PSPCurrentNumberOfRenderedSprite=0;

// Simple sprite for background (no texture)

#define MAX_SPRITESIMPLE_RENDER 10
#define SPRITESIMPLE_SIZE_IN_FLOATS 10 // Should be 8, but does not work (strange !!!)

static float PSPSpritesRect[MAX_SPRITESIMPLE_RENDER][SPRITESIMPLE_SIZE_IN_FLOATS] __attribute__((aligned(16)));

static int PSPCurrentNumberOfRenderedSpriteSimple=0;

static int displaying=0; // 1 when displaying 0 when out of display loop

int PSPsmoothpixel=1;

#define u_char unsigned char
#define u_short unsigned short
#define u_int unsigned int
#define u_long unsigned long

typedef struct tagTIM2_FILEHEADER {
        char    FileId[4];      // files ID (always "T", "I", "M", "2")
        u_char  FormatVersion;  // files format version number
        u_char  FormatId;       // Format ID
        u_short Pictures;       // Number of picture data parts
        char    Reserved[8];    // Padding (always 0x00)
} TIM2_FILEHEADER;


typedef struct tagTIM2_PICTUREHEADER {
        u_int   TotalSize;      // Total size in bytes of all picture data
        u_int   ClutSize;       // Size in bytes of CLUT data part
        u_int   ImageSize;      // Size in bytes of image data part
        u_short HeaderSize;     // Size in bytes of header part
        u_short ClutColors;     // Total number of colors in CLUT data part
        u_char  PictFormat;     // Picture format ID (always 0)
        u_char  MipMapTextures; // Number of MIPMAP textures
        u_char  ClutType;       // CLUT data part type
        u_char  ImageType;      // Image data part type
        u_short ImageWidth;     // Picture width
        u_short ImageHeight;    // Picture height
        u_long  GsTex0;         // GS TEX0 register data
        u_long  GsTex1;         // GS TEX1 register data
        u_int   GsRegs;         // GS TEXA, FBA, PABE register data
        u_int   GsTexClut;      // GS TEXCLUT register data
} TIM2_PICTUREHEADER;


void PSPDisplayImage_Reset()
{
  PSPCurrentNumberOfRenderedRectangle=0;
  PSPCurrentNumberOfRenderedSprite=0;
  PSPCurrentNumberOfRenderedSpriteSimple=0; // Background sprite
}

unsigned char testpal[] __attribute__((aligned(64))) =
{

0x01, 0x01, 0x01, 0x7f,
0x1e, 0x1e, 0x1e, 0x83,
0x45, 0x45, 0x45, 0x88, 
0x65, 0x65, 0x65, 0x82,
0x8a, 0x8a, 0x8a, 0x88,
0xbd, 0xbd, 0xbd, 0x90, 
0xd5, 0xd5, 0xd5, 0x6c,
0xd8, 0xd8, 0xd8, 0x8c,
0xeb, 0xeb, 0xeb, 0x4f, 
0xf6, 0xf6, 0xf6, 0x82,
0xfd, 0xfd, 0xfd, 0x83,
0xfe, 0xfe, 0xfe, 0x38, 
0xff, 0xff, 0xff, 0x00,
0xff, 0xff, 0xff, 0x0f, 
0xff, 0xff, 0xff, 0x21, 
0xff, 0xff, 0xff, 0x50, 
};

// Paste BITMAP structure here, for not including vice headers
typedef struct
{
  int w, h;
  const unsigned int *data;
  int streched;
} BITMAP;

// ----------------------------------------------------------------
// Name : PSPDisplayImage
// Display paletized pictures
// ----------------------------------------------------------------
void PSPDisplayImage(int x, int y, void* bitmapraw)
{
  // Bitmap contains size of sprite.
  // and pointer to tim2 texture. Texture do not have the same size as wanted sprite (squared dimensions)
  // tim2 has a header, palette and then datas
  
  // -- Get info from Tim2 datas
  int texturew, textureh;
  unsigned char* pTexture;
  unsigned char* pClut;
  int u,v;
  
  BITMAP* bitmap;
  
  if (displaying==0)
    return;
  
  if ( bitmapraw == NULL )
    return;
  
  bitmap = bitmapraw;
  
  if ( bitmap->data == NULL )
    return;
  
  TIM2_FILEHEADER* pHeader;
  TIM2_PICTUREHEADER* pPictureHeader;
  
  pHeader = (TIM2_FILEHEADER*) bitmap->data;
  pPictureHeader = (TIM2_PICTUREHEADER*) (((int)pHeader)+sizeof(TIM2_FILEHEADER));
  
  texturew = pPictureHeader->ImageWidth;
  textureh = pPictureHeader->ImageHeight;
  
  pTexture = (unsigned char*) (((int)pPictureHeader) + pPictureHeader->HeaderSize);
  pClut = (unsigned char*) (((int)pTexture) + pPictureHeader->ImageSize );
  
  // Compute UV depending on "streched" flag
  if ( bitmap->streched == 1 )
  {
    // Streched mode = all texture is displayed in new size
    u = texturew;
    v = textureh;
  }
  else
  {
    // Cropped mode = only specified part is displayed, texture is not streched, ratio is kept (use for keyboard with does not cover all texture)
    u = bitmap->w;
    v = bitmap->h;
  }
  
  // -- Set Polygon vertex and UV
  memset(PSPRectangle[PSPCurrentNumberOfRenderedRectangle], 0, QUAD_SIZE_IN_FLOATS*4);
  // Vertex position
  // X
  PSPRectangle[PSPCurrentNumberOfRenderedRectangle][2]=x;
  PSPRectangle[PSPCurrentNumberOfRenderedRectangle][7]=x+bitmap->w;
  PSPRectangle[PSPCurrentNumberOfRenderedRectangle][12]=x;
  PSPRectangle[PSPCurrentNumberOfRenderedRectangle][17]=x+bitmap->w;
  PSPRectangle[PSPCurrentNumberOfRenderedRectangle][22]=x;
  PSPRectangle[PSPCurrentNumberOfRenderedRectangle][27]=x+bitmap->w;
  // Y
  PSPRectangle[PSPCurrentNumberOfRenderedRectangle][3]=y;
  PSPRectangle[PSPCurrentNumberOfRenderedRectangle][8]=y;
  PSPRectangle[PSPCurrentNumberOfRenderedRectangle][13]=y+bitmap->h;
  PSPRectangle[PSPCurrentNumberOfRenderedRectangle][18]=y;
  PSPRectangle[PSPCurrentNumberOfRenderedRectangle][23]=y+bitmap->h;
  PSPRectangle[PSPCurrentNumberOfRenderedRectangle][28]=y+bitmap->h;
  // U (pixel values in texture)
  PSPRectangle[PSPCurrentNumberOfRenderedRectangle][0]=0;
  PSPRectangle[PSPCurrentNumberOfRenderedRectangle][5]=u;
  PSPRectangle[PSPCurrentNumberOfRenderedRectangle][10]=0;
  PSPRectangle[PSPCurrentNumberOfRenderedRectangle][15]=u;
  PSPRectangle[PSPCurrentNumberOfRenderedRectangle][20]=0;
  PSPRectangle[PSPCurrentNumberOfRenderedRectangle][25]=u;
  // V (pixel values in texture)
  PSPRectangle[PSPCurrentNumberOfRenderedRectangle][1]=0;
  PSPRectangle[PSPCurrentNumberOfRenderedRectangle][6]=0;
  PSPRectangle[PSPCurrentNumberOfRenderedRectangle][11]=v;
  PSPRectangle[PSPCurrentNumberOfRenderedRectangle][16]=0;
  PSPRectangle[PSPCurrentNumberOfRenderedRectangle][21]=v;
  PSPRectangle[PSPCurrentNumberOfRenderedRectangle][26]=v;
  
  // -- Set PSP registers

  sceGuEnable( GU_BLEND );

  sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0); // Do not do this in init or it will crash

	sceGuClutMode(GU_PSM_8888, 0, 0xff, 0); 
	sceGuClutLoad(2, (void *)pClut); // For 8888 colors, 1 unit = 8 colors. For 5551, 1 unit = 16 colors

	sceGuTexMode(GU_PSM_T4, 0, 0, 0);
  sceGuTexImage( 0, texturew, textureh, texturew, (void*)pTexture );
	sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
	sceGuTexFilter(GU_LINEAR_MIPMAP_LINEAR, GU_LINEAR);
	sceGuTexScale(1.0f,1.0f);
	sceGuTexOffset(0.0f,0.0f);

  // -- Draw rectangle
  
  sceGuDrawArray(GU_TRIANGLES, GU_TEXTURE_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 3*2, 0, PSPRectangle[PSPCurrentNumberOfRenderedRectangle]);

  PSPCurrentNumberOfRenderedRectangle++;
  
  if ( PSPCurrentNumberOfRenderedRectangle == MAX_BITMAP_RENDER ) // Circular buffer
    PSPCurrentNumberOfRenderedRectangle=0;

  sceGuDisable( GU_BLEND );

}

// Screenshot loaded name
char PSPCurrentScreenshotLoaded[512]={"\0"};

// Screenshot buffer (160*100 in 32bits)
unsigned int PSPScreenshot[512*256] __attribute__((aligned(16)));
int          PSPScreenshotx;
int          PSPScreenshoty;
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
  // Test if screenshot need to be loaded
  if ( strcmp( PSPCurrentScreenshotLoaded, filename) != 0 ) // If screenshot is not loaded
  {
    result = PngLoadImage(filename, PSPScreenshot, &PSPScreenshotx, &PSPScreenshoty);
    if (result==1)
    {
      strcpy(PSPCurrentScreenshotLoaded, filename);
      printf("Image loaded %s\n", filename);
    }
    else
    {
      PSPCurrentScreenshotLoaded[0]=0;
      return -1;
    }
  }

  // Define start X of image ... 3*8*12 is size of external bordel, 4 is size of internal border
  //#define SCREENSHOTSTARTX (480 - (3*12) - 2 - 160)
  int SCREENSHOTSTARTX;
  SCREENSHOTSTARTX = (480 - (3*12) - 2 - 160)+1;
  if ( nbchar > 29 )
   SCREENSHOTSTARTX += (nbchar-29)*8;
  //#define SCREENSHOTSTARTY ((3*12) + 2)
  #define SCREENSHOTSTARTY ((4*12) + 2)

  // Display Screenshot
  int u,v;
  sceGuEnable( GU_BLEND );  // Need to add this , when alpha test is enable
  sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0); // Do not do this in init or it will crash
  // Color of vertex is not used ...
	sceGuTexMode(GU_PSM_8888, 0, 0, 0);
	sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
	sceGuTexFilter(GU_LINEAR_MIPMAP_LINEAR, GU_LINEAR);
	sceGuTexScale(1.0f,1.0f);
	sceGuTexOffset(0.0f,0.0f);
  if ( PSPScreenshotx == 160 )
    sceGuTexImage( 0, 256, 256, 256, (void*)PSPScreenshot );
  else
    sceGuTexImage( 0, 512, 256, 512, (void*)PSPScreenshot );
	sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA); // Modulate=Work with no blending ... Replace=Work with blending, but only take the texture color!
  if ( PSPScreenshotx == 160 )
  {
    u = 160;
    v = 100;
  }
  else
  {
    u = 320;
    v = 200;
  }
  PSPSprites[PSPCurrentNumberOfRenderedSprite][0]=0; // U
  PSPSprites[PSPCurrentNumberOfRenderedSprite][1]=0; // V
  *((unsigned int*)&PSPSprites[PSPCurrentNumberOfRenderedSprite][2])=0xFFFFFFFF;
  PSPSprites[PSPCurrentNumberOfRenderedSprite][3]=SCREENSHOTSTARTX; // X
  PSPSprites[PSPCurrentNumberOfRenderedSprite][4]=SCREENSHOTSTARTY; // Y
  PSPSprites[PSPCurrentNumberOfRenderedSprite][5]=0; // Z

  PSPSprites[PSPCurrentNumberOfRenderedSprite][6]=u; // U
  PSPSprites[PSPCurrentNumberOfRenderedSprite][7]=v; // V
  *((unsigned int*)&PSPSprites[PSPCurrentNumberOfRenderedSprite][8])=0xFFFFFFFF;
  PSPSprites[PSPCurrentNumberOfRenderedSprite][9]=SCREENSHOTSTARTX+160;  // X
  PSPSprites[PSPCurrentNumberOfRenderedSprite][10]=SCREENSHOTSTARTY+100; // Y
  PSPSprites[PSPCurrentNumberOfRenderedSprite][11]=0; // Z

  sceKernelDcacheWritebackAll();
  
  sceGuDrawArray(GU_SPRITES, GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 2, 0, PSPSprites[PSPCurrentNumberOfRenderedSprite]);

  PSPCurrentNumberOfRenderedSprite++;

  if ( PSPCurrentNumberOfRenderedSprite == MAX_SPRITE_RENDER ) // Circular buffer
    PSPCurrentNumberOfRenderedSprite=0;

  sceGuDisable( GU_BLEND );
  return 0;
}

// ----------------------------------------------------------------
// Name : PSPDisplayText
// ----------------------------------------------------------------
void PSPDisplayText(int x,  int y, unsigned int color, const char* text)
{
  PSPDisplayString(x,y,color,text);
  
} 
#endif

extern FILEENT files[8192];
extern int filesnb;

int PSPcheck_system_directory=0; // If files explorer should check into system directory (for palettes)

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
      // Entry is found
      if (mode==0)
        strcpy(files[i].screenshotname, fullnamewithpath);
      else if (mode==1) // Text
      {
        strcpy(files[i].textname, fullnamewithpath);
        files[i].textposinzip=gZipCurrentFileLocation;
        files[i].textzipnumfile=gZipCurrentNumFile;
      }
      return; // end
    }
  }
}

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
    files[filesnb].textposinzip=-1;
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
    files[result].textposinzip=-1;
    filesnb++;
    //printf("Record %s at place %d (inserted) total filesnb %d", name,result,filesnb);
    return result;
  }
  
}

// ----------------------------------------------------------------
// Name : 
// ----------------------------------------------------------------
void RecordDirEntry(char* name)
{
  char currentshortname[512];
  extractname(currentshortname,name,NULL);
  strcpy(files[filesnb].name, name);
  strcpy(files[filesnb].shortname,currentshortname);
  files[filesnb].flags = 1; // DIR
  files[filesnb].screenshotname[0]=0;
  files[filesnb].textname[0]=0;
  files[filesnb].textposinzip=-1;
  filesnb++;
  return filesnb-1;
}

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

// ----------------------------------------------------------------
// ParseZipFile
// mode : 0=Parse disk or tape  1=parse extra infos (png, txt)
// ----------------------------------------------------------------
void ParseZipFile(char* zipname, int mode)
{
  unzFile fd = NULL;
  unz_file_info info;
  int ret = 0;
  int filetype=-1;
  char fullfilename[512];
  char name[512];
  int commonnfofound;   // Support GameBase nfo files (one Version.nfo files per archive)
  int filesindex[4096]; // Keep the place where filesnb are inserted (same number of max file)
  int nbrecorderfiles;
  int i;
  
  // Do not parse screenshot file
  if (stricmp(zipname,"Screenshots.zip")==0)
    return;

#ifdef WIN32
  _chdir(path_games);  
#endif

  // Attempt to open the archive
  fd = unzOpen(zipname);
  if(fd==NULL)
  {
    printf("ParseZipFile: Can not open Zip files\n");
   return;
  }

  commonnfofound=0;
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
  		if ( filetype == 1 ) // Disk name
  		{
  		  filesindex[nbrecorderfiles] = RecordFileEntry( fullfilename ); // Entries are sorted
  		  nbrecorderfiles++;
  		}
  		if ( filetype == 5 ) // the archive contain a nfo files
  		{
  		  if ( strcmp(name,"VERSION.NFO") == 0 )
  		  {
  		    commonnfofound=1; // One nfo for all filesnb in archive
  		  }
  		}
    }
    else if (mode == 1) // Parse extra infos
    {
  		if ( filetype == 3 ) // PNG
  		{
  		  SearchAndRecordExtraInfo(fullfilename, name, 0); // 0=record a screenshot info
      }    
  		else if ( filetype == 4 ) // TXT
  		{
        gZipCurrentFileLocation=-1;
  		  SearchAndRecordExtraInfo(fullfilename, name, 1); // 1=record a text info
      }    
  		else if ( filetype == 5 ) // NFO
  		{
        gZipCurrentFileLocation=unzGetFilePosInDir(fd);
        gZipCurrentNumFile=unzGetNumFile(fd);
        SearchAndRecordExtraInfo(fullfilename, name, 1); // 1=record a text info
  		}
    }

  }
  while ( unzGoToNextFile(fd) == UNZ_OK );

  unzClose(fd);
  
  // -- Test if an nfo was found
  if ( (mode==0) && commonnfofound )
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

// ----------------------------------------------------------------
// Name : PSPFillFileArray
// List all filesnb into memorystick PSPVice directory
// ----------------------------------------------------------------
void PSPFillFileArray()
{
  //char ext[4];
  char*       pTmp;
  char        fullname[2048];  
	int         filetype;
#ifndef WIN32
  SceIoDirent de;
	SceUID      fd;
#else
  HANDLE            hFile;
	WIN32_FIND_DATA   FindData;
  BOOL              bFindOK;
  char              searchname[2048];
#endif

  // -- First pass, search disc or tape filesnb (or any recognized files format by VICE). Also search for palette filesnb vpl
#ifndef WIN32
  memset( &de, 0, sizeof(SceIoDirent));
#endif
  if ( PSPcheck_system_directory == 0 )	
  {
#ifndef WIN32
    fd = sceIoDopen( path_games ) ;
#else
    strcpy(searchname, path_games);
    strcat(searchname, "/*");
    hFile   =   FindFirstFile(searchname,   &FindData);
#endif
    RecordDirEntry(".."); // Add "up" directory
	}
	else
	{
    strcpy(fullname, path_root);
    strcat(fullname, "/PSPViceDatas/c64sys/");
#ifndef WIN32
    fd = sceIoDopen( fullname ) ;
#else
    strcat(fullname, "*.*");
    hFile   =   FindFirstFile(searchname,   &FindData);   
#endif
	}
	
#ifndef WIN32
  if ( fd >= 0 )
#endif
  {
#ifndef WIN32
    while ( sceIoDread( fd, &de ) > 0 )
#else
    bFindOK = (INVALID_HANDLE_VALUE   !=   hFile);
  	while ( bFindOK )
#endif
    {
      char localfilename[512];
#ifndef WIN32
      strcpy(localfilename, de.d_name ); // Copy current parsed name
#else
      strcpy(localfilename, FindData.cFileName );
#endif

      if (!strcmp(localfilename, ".."))
      {
#ifdef WIN32
        bFindOK   =   FindNextFile(hFile,   &FindData);
#endif
  	    continue;
      }
  	
  	  if (!strcmp(localfilename, "."))
      {
#ifdef WIN32
        bFindOK   =   FindNextFile(hFile,   &FindData);
#endif
  	    continue;
      }
  	  
#ifndef WIN32
      if ( de.d_stat.st_mode == DIRECTORY )
#else
      if ( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
#endif
  	  {
        RecordDirEntry( localfilename );
#ifdef WIN32
        bFindOK   =   FindNextFile(hFile,   &FindData);
#endif
        continue;
  	  }
  	    
      if ( PSPcheck_system_directory == 0 )
      {
        // Normal case, search for any supported type of files
      		filetype = IsFileTypeRecognized(localfilename);
      		
      		if ( filetype == 1 )
      		{
      		  RecordFileEntry( localfilename );
      		}
      		else if ( filetype == 2 )
      		{
      		  // files is a zip archive, parse all filesnb inside
      		  // Record name of files with z:zipname.zip/filename
            ParseZipFile( localfilename, 0 );
      		}
      }
      else
      {
        // Special Case for Palette
        // Not allowed to load printer palette "mps803" (make a crash)
        pTmp=strrchr(localfilename,'.');
        if (pTmp)
        {
      		if ( stricmp( pTmp, ".vpl" ) == 0 )
      		{
      		  if ( stricmp( localfilename, "mps803.vpl") != 0 ) // Not this name
      		    RecordFileEntry( localfilename );
      		}
    		}
      }  		
#ifdef WIN32
      bFindOK   =   FindNextFile(hFile,   &FindData);
#endif
    } // while
#ifndef WIN32
  	sceIoDclose( fd ) ; 
#endif
  }
  
  if ( PSPcheck_system_directory != 0 )
    return;
    
  // -- Second Pass ... Search for screenshots or text infos
  // Reparse all filesnb  
#ifndef WIN32
  memset( &de, 0, sizeof(SceIoDirent));
  strcpy(fullname, path_games);
  fd = sceIoDopen( fullname ) ;
#else
  strcpy(searchname, path_games);
  strcat(searchname, "/*.*");
  hFile   =   FindFirstFile(searchname,   &FindData);
#endif

#ifndef WIN32
	if ( fd >= 0 )
#endif
  {
#ifndef WIN32
    while ( sceIoDread( fd, &de ) > 0 )
#else
    bFindOK = (INVALID_HANDLE_VALUE   !=   hFile);
  	while ( bFindOK )
#endif
  	{
      char localfilename[512];
#ifndef WIN32
      strcpy(localfilename, de.d_name ); // Copy current parsed name
#else
      strcpy(localfilename, FindData.cFileName );
#endif

  	  if (!strcmp(localfilename, ".."))
      {
#ifdef WIN32
        bFindOK   =   FindNextFile(hFile,   &FindData);
#endif
  	    continue;
      }
  	
  	  if (!strcmp(localfilename, "."))
      {
#ifdef WIN32
        bFindOK   =   FindNextFile(hFile,   &FindData);
#endif
  	    continue;
      }
  	    
   		filetype = IsFileTypeRecognized(localfilename);
    		
  		if ( filetype == 3 ) // PNG
  		{
  		  SearchAndRecordExtraInfo(localfilename, localfilename, 0); // 0=record a screenshot info
  		}
  		else if ( filetype == 4 ) // TXT
  		{
  		  SearchAndRecordExtraInfo(localfilename, localfilename, 1); // 1=record a text info
      }
  		else if ( filetype == 5 ) // NFO
  		{
  		  SearchAndRecordExtraInfo(localfilename, localfilename, 1); // 1=record a text info
  		}
  		else if ( filetype == 2 )
  		{
  		  // files is a zip archive, parse all files inside
  		  // Record name of files with z:zipname.zip/filename
        ParseZipFile( localfilename, 1 );
  		}
#ifdef WIN32
      bFindOK   =   FindNextFile(hFile,   &FindData);  // Next file
#endif
    } // while
#ifndef WIN32
  	sceIoDclose( fd ); 
#endif

  } // if
  
}

#ifndef WIN32
// ----------------------------------------------------------------
// 
// ----------------------------------------------------------------
void PSPStartDisplay()
{
		//sceGuSync(0, 0);

		sceGuStart(GU_DIRECT,list);
		//sceGuClearColor(0xFF102030); // For debug
		sceGuClearColor(0xFF000000); // Background color is black
		sceGuClear(GU_COLOR_BUFFER_BIT); 
		
		displaying=1;

    sceGuColor(0xFFFFFFFF);
    sceGuEnable(GU_TEXTURE_2D);

    sceGuDisable(GU_ALPHA_TEST); 
    sceGuDisable(GU_DEPTH_TEST);

    sceGuDisable( GU_BLEND );  
}


#define QUAD_SIZE_IN_FLOATS 30

float gVectorsLogo[60*QUAD_SIZE_IN_FLOATS];

int PSPArrayVideoMode_initialised=0;
int PSPVideoMode=0; // 0 = Normal mode, 1=FULL SCREEN - STRECHED 2=FULLSCREEN NO BORDER 3=FULL SCREEN - STRECHED - NOBORDER
int PSPViceVideoMode; // 0=PAL 1=NTSC

void DrawOptimisedFullScreenPicture()
{
  // In see through mode, the UV are the pixel coordinates (not UV values)
  // Draw picture in vertical strip of width "width"
  
  // C64 screen is 320*200 with a border of 32 pixel on each side and 32 (or 36 ?) above and below
  // PSP screen is 480*272
  
  int i;
  int screensize=480;
  int width=16; // 30 strips of 16 pixels
  int loops;
  float offsetx;
  float offsety;
  float offsetu;
  float offsetv;
  float horizontalzoom;
  float verticalzoom;
  
  float C64Height; // C64 picture height, can be 200 (PAL) or 187 (NTSC)
  float C64HeightOnPSPScreen; // 272 for PAL ... 254 fro NTSC
  float C64OffsetY; // 35=Pal 17=Ntsc
  
 
  loops = screensize / width; // number of strips. Will be use also in the display part

  if ( PSPArrayVideoMode_initialised == 0 )
  {
  	
    // Change parameters depends on PAL or NTSC display  
    if (PSPViceVideoMode==0) // Pal
    {
      C64Height = 200.0f;
      C64HeightOnPSPScreen = 272.0f;
      C64OffsetY = 35.0f;
    }
    else // Ntsc
    {
      C64Height = 200.0f;
      C64HeightOnPSPScreen = 272.0f;
      C64OffsetY = 19.0f;
    }

	// Init parameters depending on video mode
	  // Default values
	  horizontalzoom = 1.0f;
	  verticalzoom=1.0f;
	  offsetx = 0.0f;
	  offsety = 0.0f;
	  offsetu = 0.0f;
	  offsetv = 0.0f;

	if (PSPVideoMode==0)
	{
	// NORMAL SCREEN
	offsetx = 48.0f;
	}
	else if (PSPVideoMode==1)
	{

	// FULL SCREEN - STRECHED
	horizontalzoom = 384.0f/480.0f;
	}
	else if (PSPVideoMode==2)
	{
	// FULLSCREEN NO BORDER (remove up and down border, keep ratio)
	verticalzoom = C64Height / 272.0f; // 200 or 187
	offsetv = C64OffsetY;
	horizontalzoom = verticalzoom;
	offsetu = (0.5f* ((384.0f/horizontalzoom)-480.0f) ) - 4.0f;

	}  
	else if (PSPVideoMode==3)
	{
	// FULL SCREEN - STRECHED - NOBORDER
	horizontalzoom = 320.0f/480.0f;
	offsetu = 32.0f;
	verticalzoom = C64Height / 272.0f; // 200 or 187
	offsetv = C64OffsetY;
	}
	else
	{
	// NORMAL SCREEN
	offsetx = 48.0f;
	}
	  
    for (i=0; i<loops; i++)
    {
      float x1;
      float x2;
      
      x1 = (width*i)+offsetx;
      x2 = (width*(i+1))+offsetx;

      float u1;
      float u2;
      
      u1 = ((width*i) * horizontalzoom)+offsetu;
      u2 = ((width*(i+1)) * horizontalzoom)+offsetu;

	  float v1;
	  float v2;
	  
	  v1 = (0 * verticalzoom)+offsetv;
	  v2 = (C64HeightOnPSPScreen * verticalzoom)+offsetv;

      // Strip number i
      // U 
      gVectorsLogo[(i*QUAD_SIZE_IN_FLOATS)+0]=u1;
      gVectorsLogo[(i*QUAD_SIZE_IN_FLOATS)+5]=u2;
      gVectorsLogo[(i*QUAD_SIZE_IN_FLOATS)+10]=u1;
      gVectorsLogo[(i*QUAD_SIZE_IN_FLOATS)+15]=u2;
      gVectorsLogo[(i*QUAD_SIZE_IN_FLOATS)+20]=u1;
      gVectorsLogo[(i*QUAD_SIZE_IN_FLOATS)+25]=u2;
      
      // X 
      gVectorsLogo[(i*QUAD_SIZE_IN_FLOATS)+2]= x1;
      gVectorsLogo[(i*QUAD_SIZE_IN_FLOATS)+7]= x2;
      gVectorsLogo[(i*QUAD_SIZE_IN_FLOATS)+12]= x1;
      gVectorsLogo[(i*QUAD_SIZE_IN_FLOATS)+17]= x2;
      gVectorsLogo[(i*QUAD_SIZE_IN_FLOATS)+22]= x1;
      gVectorsLogo[(i*QUAD_SIZE_IN_FLOATS)+27]= x2;

      // V
      gVectorsLogo[(i*QUAD_SIZE_IN_FLOATS)+1]=v1;
      gVectorsLogo[(i*QUAD_SIZE_IN_FLOATS)+6]=v1;
      gVectorsLogo[(i*QUAD_SIZE_IN_FLOATS)+11]=v2;
      gVectorsLogo[(i*QUAD_SIZE_IN_FLOATS)+16]=v1;
      gVectorsLogo[(i*QUAD_SIZE_IN_FLOATS)+21]=v2;
      gVectorsLogo[(i*QUAD_SIZE_IN_FLOATS)+26]=v2;
    
      // Y
      gVectorsLogo[(i*QUAD_SIZE_IN_FLOATS)+3]=0;
      gVectorsLogo[(i*QUAD_SIZE_IN_FLOATS)+8]=0;
      gVectorsLogo[(i*QUAD_SIZE_IN_FLOATS)+13]=272;
      gVectorsLogo[(i*QUAD_SIZE_IN_FLOATS)+18]=0;
      gVectorsLogo[(i*QUAD_SIZE_IN_FLOATS)+23]=272;
      gVectorsLogo[(i*QUAD_SIZE_IN_FLOATS)+28]=272;
      
      // Z
      gVectorsLogo[(i*QUAD_SIZE_IN_FLOATS)+4]=0;
      gVectorsLogo[(i*QUAD_SIZE_IN_FLOATS)+9]=0;
      gVectorsLogo[(i*QUAD_SIZE_IN_FLOATS)+14]=0;
      gVectorsLogo[(i*QUAD_SIZE_IN_FLOATS)+19]=0;
      gVectorsLogo[(i*QUAD_SIZE_IN_FLOATS)+24]=0;
      gVectorsLogo[(i*QUAD_SIZE_IN_FLOATS)+29]=0;
    }
      
    PSPArrayVideoMode_initialised=1;
  	sceKernelDcacheWritebackAll();
  }
  
  
  // Display 5 strips at a time
  int loop2;
  int j;
  loop2 = (loops/5);
  for (j=0; j<loop2; j++)
  {
    void* tmp;
    tmp = (void*) (((int)gVectorsLogo)+(j*5*QUAD_SIZE_IN_FLOATS*4));
    sceGuDrawArray(GU_TRIANGLES, GU_TEXTURE_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_2D, (3*2)*5, 0, tmp);
  }
  
}

// C64 palette (ARGB)
#ifndef PSPPLUS4
unsigned int clut_face[16] __attribute__((aligned(16))) =
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
#else
// Plus4 palette
unsigned int clut_face[16] __attribute__((aligned(16))) =
{
  0xFF000000,
  0xFFFFFFFF,
  0xFF2b3768,
  0xFFb2a470,
  0xFF863d6f,
  0xFF438d58,
  0xFF792835,
  0xFF6fc7b8,
  0xFF254f6f,
  0xFF003943,
  0xFF59679a,
  0xFF444444,
  0xFF6c6c6c,
  0xFF84d29a,
  0xFFb55e6c,
  0xFF959595
}; 
#endif

// ----------------------------------------------------------------
// 
// ----------------------------------------------------------------
void PSPSetPaletteEntry(int id, unsigned char red,unsigned char green, unsigned char blue)
{
  if ( id<0 || id>15 )
    return;

  clut_face[id]=(0xFF<<24)|(blue<<16)|(green<<8)|red; // ABGR
}

// ----------------------------------------------------------------
// 
// ----------------------------------------------------------------

void PSPDisplayVideoBuffer()
{
  	sceGuTexMode(GU_PSM_T4, 0, 0, 0);
		sceGuTexFunc(GU_TFX_REPLACE,GU_TCC_RGBA); // NOTE: this enables reads of the alpha-component from the texture, otherwise blend/test won't work
  	if (PSPsmoothpixel)
  	  sceGuTexFilter(GU_LINEAR_MIPMAP_LINEAR, GU_LINEAR);
		else
		  sceGuTexFilter(GU_NEAREST,GU_NEAREST);
		sceGuTexWrap(GU_CLAMP,GU_CLAMP);
		sceGuTexScale(1,1);
		sceGuTexOffset(0,0); 

  	sceGuClutMode(GU_PSM_8888, 0, 0xff, 0);
	  sceGuClutLoad(2, (void *)clut_face); 
    
	  sceGuTexImage( 0, 512, 256, 512, (void*)ps2_videobuf );

    //sceGuEnable(GU_TEXTURE_2D);
	  
		sceGumPushMatrix();
		sceGumLoadIdentity();	  
	  
    DrawOptimisedFullScreenPicture();	  

		sceGumPopMatrix();
}


// ----------------------------------------------------------------
// 
// ----------------------------------------------------------------
void PSPWaitVBL()
{
		sceDisplayWaitVblankStart();
}

// ----------------------------------------------------------------
// 
// ----------------------------------------------------------------
void PSPSwap()
{
		sceGuSwapBuffers();
}

// ----------------------------------------------------------------
// 
// ----------------------------------------------------------------
void PSPEndDisplay()
{
    displaying=0;

    sceKernelDcacheWritebackAll(); // Write cache datas to real memory
		sceGuFinish();
		sceGuSync(0, 0);
}

// ----------------------------------------------------------------
// 
// ----------------------------------------------------------------
int PSPGetVBLCount()
{
  return sceDisplayGetVcount(); 
}

// ----------------------------------------------------------------
// 
// ----------------------------------------------------------------
void PSPChangeCpuFrequency(int freq)
{
  if (freq==222)
    scePowerSetClockFrequency(222,222,111);

  if (freq==266)
    scePowerSetClockFrequency(266,266,133);

  if (freq==333)
    scePowerSetClockFrequency(333,333,166);
}


// Font texture is 128*128 4bits
// First half is COLOR1 character, second half is 
unsigned char TextureFont[128*128/2] __attribute__((aligned(64)));

unsigned int TextureFontInitialised = 0;
int PSPFontNumber=0;

// Texture font is build from character datas (8*8bytes for each datas)
extern unsigned char Font[]; 

// Color (in clut index)
#define TEXTUREFONT_COLOR1 0 // Background (color 1 also)
#define TEXTUREFONT_COLOR2 2 // Characters with color 1 (first half)
#define TEXTUREFONT_COLOR3 3 // Characters with color 2 (second half)
#define TEXTUREFONT_COLOR4 4 // Shadow (also 5)

// Font palette (ABGR) (Only two color used 0 and 1 (0 is transparent)
const unsigned int clut_font[16] __attribute__((aligned(16))) =
{
  0x00000000,
  0x00000000,
  0xFFF0F0F0,
  0xFF30C070,
  0xFF000000,
  0xFF000000,
  0x7fE21AB4,
  0x7f1ED21F,
  0x7fAE1B21,
  0x7f0AF6DF,
  0x7f0441B8,
  0x7f04336A,
  0x7f574AFE,
  0x7f404542,
  0x7f6F7470,
  0x7f59FE59,
}; 


void PSPCreateTextureFontCreateCharacter(unsigned char* source, unsigned char* dest)
{
  int i,j;
  //unsigned char value;
  unsigned char characterrow;
  // From 8 bytes, build a character of (8/2) * 8 bytes

  // Erase Character
  for (j=0; j<8; j++)
  {
    dest[0+j*(128/2)]=0;
    dest[1+j*(128/2)]=0;
    dest[2+j*(128/2)]=0;
    dest[3+j*(128/2)]=0;

    dest[0+j*(128/2)+4096]=0;
    dest[1+j*(128/2)+4096]=0;
    dest[2+j*(128/2)+4096]=0;
    dest[3+j*(128/2)+4096]=0;
  }

  unsigned char tempcharacter[8][8]; // Compute a temporary character

  for (i=0; i<8; i++)
    for (j=0; j<8; j++)
      tempcharacter[i][j]=0;
    

  for (j=0; j<8; j++) // 8 lines
  {
    // Process width of character
  	characterrow=source[j]; // Get the byte which contains value of a row of 8 pixels

    // Compute shadow
    if (j != 7 )
    {
      if ( ( characterrow & (1<<7) ) != 0 ) tempcharacter[j+1][1] = TEXTUREFONT_COLOR4;
      if ( ( characterrow & (1<<6) ) != 0 ) tempcharacter[j+1][2] = TEXTUREFONT_COLOR4;
      if ( ( characterrow & (1<<5) ) != 0 ) tempcharacter[j+1][3] = TEXTUREFONT_COLOR4;
      if ( ( characterrow & (1<<4) ) != 0 ) tempcharacter[j+1][4] = TEXTUREFONT_COLOR4;
      if ( ( characterrow & (1<<3) ) != 0 ) tempcharacter[j+1][5] = TEXTUREFONT_COLOR4;
      if ( ( characterrow & (1<<2) ) != 0 ) tempcharacter[j+1][6] = TEXTUREFONT_COLOR4;
      if ( ( characterrow & (1<<1) ) != 0 ) tempcharacter[j+1][7] = TEXTUREFONT_COLOR4;
    }
  
    // Character on first half of texture
    if ( ( characterrow & (1<<7) ) != 0 ) tempcharacter[j][0] = TEXTUREFONT_COLOR2;
    if ( ( characterrow & (1<<6) ) != 0 ) tempcharacter[j][1] = TEXTUREFONT_COLOR2;
    if ( ( characterrow & (1<<5) ) != 0 ) tempcharacter[j][2] = TEXTUREFONT_COLOR2;
    if ( ( characterrow & (1<<4) ) != 0 ) tempcharacter[j][3] = TEXTUREFONT_COLOR2;
    if ( ( characterrow & (1<<3) ) != 0 ) tempcharacter[j][4] = TEXTUREFONT_COLOR2;
    if ( ( characterrow & (1<<2) ) != 0 ) tempcharacter[j][5] = TEXTUREFONT_COLOR2;
    if ( ( characterrow & (1<<1) ) != 0 ) tempcharacter[j][6] = TEXTUREFONT_COLOR2;
    if ( ( characterrow & (1<<0) ) != 0 ) tempcharacter[j][7] = TEXTUREFONT_COLOR2;
  }  

  
  // Copy character into texture
  for (j=0; j<8; j++) // 8 lines
  {
    dest[0]=tempcharacter[j][0]+(tempcharacter[j][1]*16);
    dest[1]=tempcharacter[j][2]+(tempcharacter[j][3]*16);
    dest[2]=tempcharacter[j][4]+(tempcharacter[j][5]*16);
    dest[3]=tempcharacter[j][6]+(tempcharacter[j][7]*16);

    // Remplace colors
    for (i=0; i<8; i++)
    {
      // This fail !!!!
      //unsigned char value;
      //value = tempcharacter[j][i];
      //if ( value == TEXTUREFONT_COLOR2 ) // This line fails !!! 
      //{
      //   tempcharacter[j][i] = TEXTUREFONT_COLOR3;
      //}
      
      // Increment all colors by 1 ... palette is done to result in targeting the good colors
      // I use this because the above method leads to crash
      tempcharacter[j][i] += 1;
    }
      
    // Fill second half of texture
    dest[0+4096]=tempcharacter[j][0]+(tempcharacter[j][1]*16);
    dest[1+4096]=tempcharacter[j][2]+(tempcharacter[j][3]*16);
    dest[2+4096]=tempcharacter[j][4]+(tempcharacter[j][5]*16);
    dest[3+4096]=tempcharacter[j][6]+(tempcharacter[j][7]*16);

    dest += (128/2); // Go to next line
  }  
  

}

// ----------------------------------------------------------------
// Create texture font
// ----------------------------------------------------------------
void PSPCreateTextureFont()
{
  int i;
  unsigned char* dest;
  // Format of original font datas : 8x8 pixel, 1 color, 96 caracters.
  // 1 byte represent 8 pixels (one pixel per bit)
  // 8 bytes represents 1 caracter.
  // The character order is the same as ASCII table.
  // The first character correspond to ASCII caracter 32 (space).
  // For each character, create a 8x8 block into texture
  for (i=0; i<96; i++)
  {
    // Get position in dest texture (texture is 128 pixel width, 16 character can fit)
    // So dest adress is row where character is * (size of a character row) + colom * (size of a character)
    // Width of a character is (8/2) = 4 bytes
    // Size of a full character row (8*(128/2)) = 512 bytes
    dest = & ( TextureFont[ ((i/16) * 512) + (i%16)*4 ] );
    // Add character
    PSPCreateTextureFontCreateCharacter( &(Font[ PSPFontNumber*(96*8) + i*8 ]), dest);
  }
}

// ----------------------------------------------------------------
// Display a string using a texture font
// ----------------------------------------------------------------
void PSPDisplayString(int x, int y, unsigned int color, char* string)
{
  if (displaying==0)
    return;

  if (string[0]==0)
    return;
    
  if ( TextureFontInitialised == 0 )
  {
    PSPCreateTextureFont();
    TextureFontInitialised = 1;
  }

  // Display test texture
  //sceGuTexImage( 0, 128, 128, 128, (void*)TextureFont );
  // -- Draw rectangle
  //sceGuDrawArray(GU_SPRITES, GU_TEXTURE_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 2, 0, gSprite);
  
  // Parse all character and display them (do not display space)
  int lenght;
  int i;
  lenght = strlen(string);

  int v_offset; // Font texture have 2 parts, we can choose color of fonte (because color of fonte do not work)

  if (color==0xF0F0F0)
    v_offset=0;
  else
    v_offset=64; // Start at half of screen

  sceGuEnable( GU_BLEND );  // Need to add this , when alpha test is enable

  sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0); // Do not do this in init or it will crash

  // Color of vertex is not used ...
	sceGuTexMode(GU_PSM_T4, 0, 0, 0);
	sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
	sceGuTexFilter(GU_LINEAR_MIPMAP_LINEAR, GU_LINEAR);
	sceGuTexScale(1.0f,1.0f);
	sceGuTexOffset(0.0f,0.0f);
  
  sceGuTexImage( 0, 128, 128, 128, (void*)TextureFont );
	sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA); // Modulate=Work with no blending ... Replace=Work with blending, but only take the texture color!

  // Change color of clut texture

	sceGuClutMode(GU_PSM_8888, 0, 0xff, 0); 
	sceGuClutLoad(2, clut_font); // For 8888 colors, 1 unit = 8 colors. For 5551, 1 unit = 16 colors

  //sceGuEnable(GU_TEXTURE_2D);

  for (i=0; i<lenght; i++)
  {
    unsigned char value;
    value = string[i];
    
    if ( (value != 0) && (value != ' ') )
    {
      // Add character to render
      // Compute UV values
      int u,v;
      
      u = ((value-' ')%16)*8;
      v = ((value-' ')/16)*8;
      
      PSPSprites[PSPCurrentNumberOfRenderedSprite][0]=u; // U
      PSPSprites[PSPCurrentNumberOfRenderedSprite][1]=v+v_offset; // V
      *((unsigned int*)&PSPSprites[PSPCurrentNumberOfRenderedSprite][2])=color;
      PSPSprites[PSPCurrentNumberOfRenderedSprite][3]=x; // X
      PSPSprites[PSPCurrentNumberOfRenderedSprite][4]=y; // Y
      PSPSprites[PSPCurrentNumberOfRenderedSprite][5]=0; // Z

      PSPSprites[PSPCurrentNumberOfRenderedSprite][6]=u+8; // U
      PSPSprites[PSPCurrentNumberOfRenderedSprite][7]=v+8+v_offset ; // V
      *((unsigned int*)&PSPSprites[PSPCurrentNumberOfRenderedSprite][8])=color;
      PSPSprites[PSPCurrentNumberOfRenderedSprite][9]=x+8; // X
      PSPSprites[PSPCurrentNumberOfRenderedSprite][10]=y+8; // Y
      PSPSprites[PSPCurrentNumberOfRenderedSprite][11]=0; // Z

      sceKernelDcacheWritebackAll();
      
      sceGuDrawArray(GU_SPRITES, GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 2, 0, PSPSprites[PSPCurrentNumberOfRenderedSprite]);
    
      x=x+8;
      PSPCurrentNumberOfRenderedSprite++;

      if ( PSPCurrentNumberOfRenderedSprite == MAX_SPRITE_RENDER ) // Circular buffer
        PSPCurrentNumberOfRenderedSprite=0;
    }
    else if ( value==' ') 
    {
      x=x+8;
    }
    
  }

  sceGuDisable( GU_BLEND );

  //sceGuColor(0xffffffff);
}

unsigned int PSPFillColor=0xFF000000; // ARGB

void PSPSetFillColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A)
{
  PSPFillColor = (A<<24) + (B<<16) + (G<<8) + R;
}



void PSPFillRectangle(unsigned int x, unsigned int y, unsigned int x2, unsigned int y2)
{
  // -- Set Datas
  
  *((unsigned int*)&PSPSpritesRect[PSPCurrentNumberOfRenderedSpriteSimple][0])=PSPFillColor;
  PSPSpritesRect[PSPCurrentNumberOfRenderedSpriteSimple][1]=(float)x;
  PSPSpritesRect[PSPCurrentNumberOfRenderedSpriteSimple][2]=(float)y;
  PSPSpritesRect[PSPCurrentNumberOfRenderedSpriteSimple][3]=0.0f;
  *((unsigned int*)&PSPSpritesRect[PSPCurrentNumberOfRenderedSpriteSimple][4])=PSPFillColor;
  PSPSpritesRect[PSPCurrentNumberOfRenderedSpriteSimple][5]=(float)x2;
  PSPSpritesRect[PSPCurrentNumberOfRenderedSpriteSimple][6]=(float)y2;
  PSPSpritesRect[PSPCurrentNumberOfRenderedSpriteSimple][7]=0.0f;

  // -- Set PSP registers

  sceGuEnable( GU_BLEND );  // Need to add this , when alpha test is enable
  //sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0); // Do not do this in init or it will crash
  sceGuDisable( GU_TEXTURE_2D);

  // -- Draw rectangle
  
  sceGuDrawArray(GU_SPRITES, GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 2, 0, PSPSpritesRect[PSPCurrentNumberOfRenderedSpriteSimple]);

  PSPCurrentNumberOfRenderedSpriteSimple++;

  sceGuDisable( GU_BLEND );
  sceGuEnable(GU_TEXTURE_2D);
}

// ---------------------------------------------------------------
void PSPGetDateAndTime(char* buffer)
{
  int i;
  for (i=0; i<1024; i++)
    buffer[i]=0;
  pspTime time;
  sceRtcGetCurrentClockLocalTime(&time); 
  sprintf(buffer,"%d/%d/%d %dH%02d\n", time.day, time.month,time.year, time.hour, time.minutes);
}

#endif // Win32

// Infos about "Texture functions"
/*
void sceGuTexFunc  (  int  tfx,  int  tcc )   
 
   Set how textures are applied. 

Key for the apply-modes:

Cv - Color value result
Ct - Texture color
Cf - Fragment color
Cc - Constant color (specified by sceGuTexEnvColor())
Available apply-modes are: (TFX)

GU_TFX_MODULATE - Cv=Ct*Cf TCC_RGB: Av=Af TCC_RGBA: Av=At*Af
GU_TFX_DECAL - TCC_RGB: Cv=Ct,Av=Af TCC_RGBA: Cv=Cf*(1-At)+Ct*At Av=Af
GU_TFX_BLEND - Cv=(Cf*(1-Ct))+(Cc*Ct) TCC_RGB: Av=Af TCC_RGBA: Av=At*Af
GU_TFX_REPLACE - Cv=Ct TCC_RGB: Av=Af TCC_RGBA: Av=At
GU_TFX_ADD - Cv=Cf+Ct TCC_RGB: Av=Af TCC_RGBA: Av=At*Af
The fields TCC_RGB and TCC_RGBA specify components that differ between the two different component modes.


GU_TFX_MODULATE - The texture is multiplied with the current diffuse fragment
GU_TFX_REPLACE - The texture replaces the fragment
GU_TFX_ADD - The texture is added on-top of the diffuse fragment
Available component-modes are: (TCC)

GU_TCC_RGB - The texture alpha does not have any effect
GU_TCC_RGBA - The texture alpha is taken into account

Parameters:
 tfx  - Which apply-mode to use  
 tcc  - Which component-mode to use 
*/

