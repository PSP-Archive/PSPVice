#include <stdio.h>

#ifndef WIN32
#include "types.h"
#include <string.h>
#include "osdmsg.h"
#include "bitmap.h"
#include "infowin.h"
#include "support.h"
#include "pad.h"
#else
#include "Win32pad.h"
#endif

#include "joy.h"
#include "bitmap.h"
#include "menu.h"
#include "fileexp.h"
#include "resources.h"

#ifdef WIN32
#include "Win32PSPSpecific.h"
#else
#include "PSPSpecific.h"
#endif

#include "PSPFileSystem.h"

extern int disp_w;
extern int disp_h;
extern char path_games[];
extern int true_drive_emulation;
extern int gZipCurrentFileLocation;
extern int gZipCurrentNumFile;
char gIsDirReadOnce=0;

// In full screen, a C64 caracter is 12 pixels big
#define SELIW      200
#define WINW       (480-(2*3*12))
#define WINH       (272-(2*3*12))
#define WINX       ((disp_w-WINW)/2)
#define WINY       ((disp_h-WINH)/2)
#define PERPAGE    15
#define SBXSTART   (WINX+WINW)
#define SBXEND     (WINX+WINW+8)
#define CARAHEIGHT 12

extern int is_paused;

int file_explorer_active = 0;
static void (*callback)(unsigned int, void*) = NULL;
static void *callbackparam = NULL;

#define F_DIR 1

// Array that stores files
FILEENT files[4096]; // Max number of filesnb 
int     filesnb = 0;

static int selected = 0;

char SelectedTitleInfos[15][40];     // 15 lines of infos (For first 8, if screenshot is there 18 char will be displayed)

char SelectedTitleNameReadFromNFO[512];   // If a game name has been found into NFO file (Example : "Yie Ar Kung-Fu" in YIEAR1_08821_02.zip)
char SelectedTitleScreenshotFromNFO[128]; // If a screenshot name has been found into NFO file (Example : "Y\Yie_Ar_Kung_Fu.png")
char SelectedTitleExt[16]; // Store extension of files (T64, D64)
int  SelectedTitleJoystickPortFromNFO; // If into nfo text we found info about joystick port (-1=None 1=Joy port 1, 2=Joy Port2)
int  SelectedTitleTDUFromNFO; // If into nfo text we found info about TDU (True drive emulation)

char readdirasked=0; // 0=not asked,  1=asked and displayed 2=dir is read

int  waitbeforereadscreenshot=0; // If > 0 then wait before loading screenshot

// ------------------------------------------------------------
// Name : readdir_ask
// ------------------------------------------------------------
void readdir_ask()
{
  readdirasked=1;
  show_msg("Reading directory...");
}

// ------------------------------------------------------------
// Name : sort 
// Quick Sort
// Call this from element 0 to element max-1
// Function taken from snes9x/psp (http://svn.ps2dev.org )
// ------------------------------------------------------------
void sort(int left, int right)
{
  FILEENT tmp, pivot;
  int i, p;

  if (left < right)
  {
    pivot = files[left];
    p = left;
    // Search for first element that not in order (from left)
    for (i=left+1; i<=right; i++)
    {
      if (stricmp(files[i].shortname,&pivot.shortname)<0)
      {
        p=p+1;
        tmp=files[p];
        files[p]=files[i];
        files[i]=tmp;
      }
    }
    files[left] = files[p];
    files[p] = pivot;
    sort(left, p-1);
    sort(p+1, right);
  }
}

// ------------------------------------------------------------
// Name : readdir
// ------------------------------------------------------------
int readdir()
{
  selected = 0;
  filesnb = 0;
  PSPFillFileArray(); // Call plateform specific fonction for reading directory
  if ( filesnb < 50 ) // Sort all files, only with few entries (crash on big directories)
    sort(0,filesnb-1);
  return filesnb;
}

extern int PSPcheck_system_directory;

// ------------------------------------------------------------
// Name : launch_file_explorer
// ------------------------------------------------------------
void launch_file_explorer(void (*callbac)(), void *callbackpara)
{
  file_explorer_active = 1;
  PSPcheck_system_directory=0; // Check in game directory by default
  selected = 0;
  callback = (void(*)(unsigned int, void*)) callbac;
  callbackparam = callbackpara;

  if ( (callbackparam!=NULL) && ( (int)callbackparam==0xFF01) )
    PSPcheck_system_directory=1;

  if ( !gIsDirReadOnce ) // Only first time we read dir. Keep all data for next time we enter menu
  {
    readdir_ask();
    gIsDirReadOnce=1;
  }
}

// ------------------------------------------------------------
// Name : draw_scrollbar
// ------------------------------------------------------------
void draw_scrollbar()
{
  int hei;
  int start;

  hei = (PERPAGE/(float)filesnb)*WINH+0.5f; // Heigh of cursor
  start = (selected/(float)filesnb)*(WINH-hei)+0.5f; // position of cursor

  if (hei < WINH)
  {
    PSPSetFillColor(80, 80, 80,192);
    PSPFillRectangle(SBXSTART, WINY, SBXEND, WINY+WINH);
    PSPSetFillColor(180, 180, 180,192);
    PSPFillRectangle(SBXSTART, WINY+start, SBXEND, WINY+hei+start);
  }
}

// ------------------------------------------------------------
// Name : go_upper
// Go upper into directory
// ------------------------------------------------------------
void go_upper()
{
  char* pTmp;

  if ( strlen(path_games) <= 5 ) // If at memory stick root (ms0:/) then do not go up.
    return;

  // Remove last "/"
  pTmp = strrchr(path_games,'/');
  if (pTmp)
    pTmp[0]=0; 

  // Go to previous "/" and remove name
  pTmp = strrchr(path_games,'/');
  if (pTmp)
    pTmp[1]=0; 
}

// ------------------------------------------------------------
// Name : enter_dir
// ------------------------------------------------------------
static void enter_dir(char *name)
{
  strcat(path_games, name);
  strcat(path_games, "/");
}


// ------------------------------------------------------------
// Name : extractname
// Remove path and extension
// ------------------------------------------------------------
void extractname(char* result, char* source, char* ext)
{
  char* pTmp;
  if ( strcmp(source,"..") == 0 ) // "Up" Directory
  {
    strcpy(result,source);
    if (ext)
      ext[0]=0;
    return;
  }

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


#define INFOTEXTMAXSIZE 8192

char InfoText[INFOTEXTMAXSIZE]; // Buffer when text is read

char InfoTextCurrentLoadedTextName[512]={"\0"};
int  InfoTestCurrentLenght;  // Size of whole text
char* InfoTextCurrentPointer; // Line currently pointing to
char* InfoTextStartPointer;   // First caracter of bottom infos lines

// ------------------------------------------------------------
// Name : TestAndSkipEndOfLineChar
// Return true, if end of line was pointed
// ------------------------------------------------------------
int TestAndSkipEndOfLineChar()
{
  int  result;
  result=0;

  if ( InfoTextCurrentPointer >= InfoText+InfoTestCurrentLenght )
    return 0;

  while (    (InfoTextCurrentPointer[0]==0x0D)
    || (InfoTextCurrentPointer[0]==0x0A) )
  {
    InfoTextCurrentPointer++;
    result=1;
  }
  return result;
}

// ------------------------------------------------------------
// Name : SkipOneLine
// ------------------------------------------------------------
void SkipOneLine()
{
  while (    (InfoTextCurrentPointer[0]!=0x0D)
    && (InfoTextCurrentPointer[0]!=0x0A)
    && InfoTextCurrentPointer < InfoText+InfoTestCurrentLenght )
  {
    InfoTextCurrentPointer++;
  }
  TestAndSkipEndOfLineChar();
}

// ------------------------------------------------------------
// Name : GetALine
// Used for normal text
// Read text from current position (InfoTextCurrentPointer) and get a line
// If no line, write null line
// ------------------------------------------------------------
void GetALine(char* dest, int linemaxcaracter )
{
  char* desstart;
  int copiedchars;

  if (InfoTextCurrentPointer[0]==0) // If end of files is reached
  {
    dest[0]=0;
    return;
  }
  copiedchars=0;
  desstart=dest; // Store destination start
  // Copy caracters one by one, stops if when meets carriage return or limit reached.
  while ( !TestAndSkipEndOfLineChar() && (copiedchars<linemaxcaracter) )
  {
    dest[copiedchars]=InfoTextCurrentPointer[0];
    copiedchars++;
    InfoTextCurrentPointer++;
  }
  dest[copiedchars]=0;

  // Make a test on string, to detect some automatic options
  if ( stricmp(dest, "Joystick Port 1") == 0)
  {
    // Automatic set joystick in port 1
    SelectedTitleJoystickPortFromNFO=1;
  }
  else if ( stricmp(dest, "Joystick Port 2") == 0)
  {
    SelectedTitleJoystickPortFromNFO=2;
  }
}

// ------------------------------------------------------------
// Name : NfoSearchKey
// Parse all NFO buffer and search string starting at new line
// Name can only be start of a work (example "Nam" and will find "Name:"
// ------------------------------------------------------------
char* NfoSearchKey(char* name)
{
  int   currentposition;
  char* pTmp;

  currentposition=0;

  while(currentposition<InfoTestCurrentLenght)
  {
    // Check if we found key name
    if ( strncmp(&InfoText[currentposition], name, strlen(name) ) == 0 )
    {
      // Search for ":"
      while ( InfoText[currentposition]!=':' && InfoText < InfoText+InfoTestCurrentLenght && InfoText[currentposition]!=0x0D)
      {
        currentposition++;
      }
      // If ':' was found then continue searching for key
      if ( InfoText[currentposition]==':' )
      {
        currentposition++;
        // Skip all space or tab characters
        while ( (InfoText[currentposition]==' ' || InfoText[currentposition]=='\t') && InfoText < InfoText+InfoTestCurrentLenght)
        {
          currentposition++;
        }
        // IF (Unknown) or (None) then return NULL
        if ( strnicmp(&InfoText[currentposition],"(Unkn",5)==0)
          return NULL;
        if ( strnicmp(&InfoText[currentposition],"(None",5)==0)
          return NULL;

        return &InfoText[currentposition];
      }
      else
      {
        // ':' is not found, then do nothing, search for another key
      }
    }
  
    // Go end of line, or end of file
    while (   (InfoText[currentposition]!=0x0D)
           && (InfoText[currentposition+1]!=0x0A)
           && InfoText < InfoText+InfoTestCurrentLenght )
    {
      currentposition++;
    }
    // Jump over end of line characters (go to next line)
    if ( InfoText[currentposition]==0x0d && InfoText[currentposition+1]==0x0A )
    {
      currentposition +=2;
    }
  }
  return NULL; // Not found
}


// ------------------------------------------------------------
// Name : NfoCopyString
// Copy all chars from src until end of line (or end of file)
// Limit to 40 characters
// Return NULL if all has been copied
// Else return non null char pointer
// ------------------------------------------------------------
char* NfoCopyString(char* dest, char* src, int maxsize)
{
    int nbchars;
    nbchars=0;

    // Go end of line, or end of file
    while (   (src[0]!=0x0D)
           && (src[0]!=0x0A)
           && src < InfoText+InfoTestCurrentLenght
           && nbchars < maxsize )
    {
      dest[0]=src[0];
      src++;
      dest++;
      nbchars++;
    }

    dest[0]=0; // End of line

    if ( nbchars == maxsize )
      return src;
    else
      return NULL;
}

// ------------------------------------------------------------
// Name : IsNfoFile
// ------------------------------------------------------------
int IsNfoFile(char* name)
{
  int result;
  char* pTmp;
  result=0;
  pTmp=strrchr(name, '.');
  if (pTmp)
  {
    if (stricmp(pTmp,".nfo")==0)
      result=1;
  }
  return result;
}

// ------------------------------------------------------------
// ------------------------------------------------------------
void ResetNFOAutomaticInfosStored()
{
  SelectedTitleNameReadFromNFO[0]=0; // Erase Name read from NFO (optionnal)
  SelectedTitleScreenshotFromNFO[0]=0; // Erase screenshot read from NFO (optionnal)
  SelectedTitleJoystickPortFromNFO=-1;
  SelectedTitleTDUFromNFO=0; // No
  InfoTextCurrentLoadedTextName[0]=0;
}

// ------------------------------------------------------------
// If text is not loaded then load it and initialise display text
// ------------------------------------------------------------
void LoadAndFillTextInfoArray(char* name)
{
  int   i,j;
  FILE* files;
  char* pTmp;
  int   currentusedline; // We fill the 14 line of info block one by one
  int remainingbytes;

  // Check if text is loaded
  if ( strcmp( InfoTextCurrentLoadedTextName, name ) == 0 )  // Name are equal
  {
    //printf("Name are equals, skiping txt load and init\n");
    return; // Do nothing, all is already done.
  }

  ResetNFOAutomaticInfosStored();

  // -- Load files
  files=fopen(name,"r");
  if (files==NULL)
    return;
  for (i=0; i<INFOTEXTMAXSIZE; i++)
    InfoText[i]=0;  
  InfoTestCurrentLenght=fread(InfoText,1,INFOTEXTMAXSIZE,files);
  printf("files read %s, size %d\n", name, InfoTestCurrentLenght);
  fclose(files);

  // Store current loaded files name
  strcpy(InfoTextCurrentLoadedTextName,name);

  // Analyse text
  InfoTextCurrentPointer = InfoText;

  if ( !IsNfoFile(name ) )
  {
    // Text is simple text. 7 line of credits, and then infos or manual
    GetALine(SelectedTitleInfos[0], 19);
    GetALine(SelectedTitleInfos[1], 19);
    GetALine(SelectedTitleInfos[2], 19);
    GetALine(SelectedTitleInfos[3], 19);
    GetALine(SelectedTitleInfos[4], 19);
    GetALine(SelectedTitleInfos[5], 19);
    GetALine(SelectedTitleInfos[6], 19);
    GetALine(SelectedTitleInfos[7], 19);

    InfoTextStartPointer = InfoTextCurrentPointer;

    GetALine(SelectedTitleInfos[8], 38);
    GetALine(SelectedTitleInfos[9], 38);
    GetALine(SelectedTitleInfos[10], 38);
    GetALine(SelectedTitleInfos[11], 38);
    GetALine(SelectedTitleInfos[12], 38);
    GetALine(SelectedTitleInfos[13], 38);
    GetALine(SelectedTitleInfos[14], 38);
  }
  else
  {
    // Empty all lines
    for (i=0; i<15; i++)
    {
      for (j=0; j<40; j++)
        SelectedTitleInfos[i][j]=0;
    }
    currentusedline=0;

    printf("Display nfo ...\n");

    // Search for game name
    pTmp=NfoSearchKey("Name");
    if (pTmp)
      NfoCopyString(SelectedTitleNameReadFromNFO, pTmp,511); // Copy from pTmp to end of line

    // Search for screenshot name
    pTmp=NfoSearchKey("Screen");
    if (pTmp)
    {
      char  result[128];
      char* pName;
      int i;
      NfoCopyString(result, pTmp,128); // Copy from pTmp to end of line
      // Change \ into /
      i=0;
      while (result[i]!=0)
      {
        if ( result[i] == '\\' )
          result[i]='/';
        i++;
      }
      // Copy only name
      pName=strrchr(result,'/');
      if (pName)
      {
        pName +=1;
      }
      else
        pName=result;

      strcpy(SelectedTitleScreenshotFromNFO,"z:Screenshots.zip/");
      strcat(SelectedTitleScreenshotFromNFO,pName);
      waitbeforereadscreenshot=40; // Wait 40 frames before loading screenshot
    }

    // Search if TDU is needed
    pTmp=NfoSearchKey("True");
    if (pTmp)
    {
      if ( strnicmp(pTmp, "Yes",3) == 0)
      {
        SelectedTitleTDUFromNFO=1; // Yes
      }
    }


    // If no screenshots, use 40 characters
    if ( SelectedTitleScreenshotFromNFO[0]==0)
    {
      // Seach for year and publisher
      pTmp=NfoSearchKey("Publ");
      if (pTmp)
      {
        NfoCopyString(SelectedTitleInfos[currentusedline], pTmp,38); // Copy from pTmp to end of line
        currentusedline++;
      }
      pTmp=NfoSearchKey("Year");
      if (pTmp)
      {
        NfoCopyString(SelectedTitleInfos[currentusedline], pTmp,38); // Copy from pTmp to end of line
        currentusedline++;
      }
      // Seach for prog
      pTmp=NfoSearchKey("Prog");
      if (pTmp)
      {
        NfoCopyString(SelectedTitleInfos[currentusedline], pTmp,38); // Copy from pTmp to end of line
        currentusedline++;
      }
      // Seach for musi
      pTmp=NfoSearchKey("Musi");
      if (pTmp)
      {
        NfoCopyString(SelectedTitleInfos[currentusedline], pTmp,38); // Copy from pTmp to end of line
        currentusedline++;
      }
      // Seach for Genre
      pTmp=NfoSearchKey("Genre");
      if (pTmp)
      {
        NfoCopyString(SelectedTitleInfos[currentusedline], pTmp,38); // Copy from pTmp to end of line
        currentusedline++;
      }
      // Seach for Players
      pTmp=NfoSearchKey("Play");
      if (pTmp)
      {
        NfoCopyString(SelectedTitleInfos[currentusedline], pTmp,38); // Copy from pTmp to end of line
        currentusedline++;
      }
      // Seach for Controls
      pTmp=NfoSearchKey("Contro");
      if (pTmp)
      {
        // Make a test on string, to detect some automatic options
        if ( strnicmp(pTmp, "Joystick Port 1",15) == 0)
        {
          // Automatic set joystick in port 1
          SelectedTitleJoystickPortFromNFO=1;
        }
        else if ( strnicmp(pTmp, "Joystick Port 2",15) == 0)
        {
          SelectedTitleJoystickPortFromNFO=2;
        }
        NfoCopyString(SelectedTitleInfos[currentusedline], pTmp,38); // Copy from pTmp to end of line
        currentusedline++;
      }
    }
    else
    {
      // IF screenshots, we are going to concat all string
      char allstring[512];
      char stringtemp[256];
      // Seach for year and publisher
      pTmp=NfoSearchKey("Publ");
      if (pTmp)
      {
        NfoCopyString(stringtemp, pTmp,256);
        strcpy(allstring,stringtemp);
        strcat(allstring, " / ");
      }
      pTmp=NfoSearchKey("Year");
      if (pTmp)
      {
        NfoCopyString(stringtemp, pTmp,256);
        strcat(allstring,stringtemp);
        strcat(allstring, " / ");
      }
      // Seach for prog
      pTmp=NfoSearchKey("Prog");
      if (pTmp)
      {
        NfoCopyString(stringtemp, pTmp,256);
        strcat(allstring,stringtemp);
        strcat(allstring, " / ");
      }
      // Seach for musi
      pTmp=NfoSearchKey("Musi");
      if (pTmp)
      {
        NfoCopyString(stringtemp, pTmp,256);
        strcat(allstring,stringtemp);
        strcat(allstring, " / ");
      }
      // Seach for Genre
      pTmp=NfoSearchKey("Genre");
      if (pTmp)
      {
        NfoCopyString(stringtemp, pTmp,256);
        strcat(allstring,stringtemp);
        strcat(allstring, " / ");
      }
      // Seach for Players
      pTmp=NfoSearchKey("Play");
      if (pTmp)
      {
        NfoCopyString(stringtemp, pTmp,256);
        strcat(allstring,stringtemp);
        strcat(allstring, " / ");
      }
      // Seach for Controls
      pTmp=NfoSearchKey("Contro");
      if (pTmp)
      {
        // Make a test on string, to detect some automatic options
        if ( strnicmp(pTmp, "Joystick Port 1",15) == 0)
        {
          // Automatic set joystick in port 1
          SelectedTitleJoystickPortFromNFO=1;
        }
        else if ( strnicmp(pTmp, "Joystick Port 2",15) == 0)
        {
          SelectedTitleJoystickPortFromNFO=2;
        }
        NfoCopyString(stringtemp, pTmp,256);
        strcat(allstring,stringtemp);
        strcat(allstring, " / ");
      }
      // Seach for Langage
      pTmp=NfoSearchKey("Lang");
      if (pTmp)
      {
        NfoCopyString(stringtemp, pTmp,256);
        strcat(allstring,stringtemp);
        //strcat(allstring, " / ");
      }
      // -- All info are inside stringtemp, no cut it on lines
      remainingbytes=0;
      remainingbytes=strlen(allstring);
      pTmp=allstring;
      for (i=0; i<8; i++)
      {
        if (remainingbytes>0)
        {
          strncpy(SelectedTitleInfos[currentusedline],pTmp,18); 
          remainingbytes-=18;
          pTmp+=18;
        }
        currentusedline++;
      }
    }

    // Jump to second part if not done
    if ( currentusedline < 8 )
      currentusedline=8;

    // Loop on comments line and fill all available lines
    // Seach for Comments
    pTmp=NfoSearchKey("Comment");
    if (pTmp && pTmp[0]!=0x0d)
    {
      do
      {
        pTmp=NfoCopyString(SelectedTitleInfos[currentusedline], pTmp,38); // Copy from pTmp to end of line
        currentusedline++;
      } while ( pTmp!=NULL && currentusedline!=15 );
    }

    // Loop on comments line and fill all available lines
    // Seach for Comments
    pTmp=NfoSearchKey("NOTE");
    if (pTmp && currentusedline<15 )
    {
      char* pResult;
      do
      {
        int nbcopiedchar=0;
        while ( nbcopiedchar<38 && pTmp<InfoText+InfoTestCurrentLenght )
        {
          if (pTmp[0]!='-' && pTmp[0]!=0x0d && pTmp[0]!=0x0a && pTmp[0]!='=' )
          {
            SelectedTitleInfos[currentusedline][nbcopiedchar]=pTmp[0];
            nbcopiedchar++;
          }
          pTmp++;
        }
        currentusedline++;
      } while ( currentusedline < 14 && pTmp<InfoText+InfoTestCurrentLenght);
    }

  }
}

// ------------------------------------------------------------
// Get new lines for bottom text display
// ------------------------------------------------------------
void GetNewDisplayInfoLines()
{
}

// ------------------------------------------------------------
// Name : file_explorer_frame
// Update and display
// ------------------------------------------------------------
void file_explorer_frame()
{
  int i, f;
  char ext[4];
  static int repeatcntr = 0;
  static int repeatspeed = 0;
  static int currentothersnamelenght = 40; // This is the current display lenght (to create a transition effect)
  static int wantedothersnamelenght = 40; // In this value we write the desired final lenght

  PSPSetFillColor(20, 20, 20, 192);
  PSPFillRectangle(WINX, WINY, WINX+WINW, WINY+WINH);

  if ( readdirasked>0 ) // This is differed to let the info windows beeing diplaid "Reading directory"
  {
    if (readdirasked==1)
    {
      show_msg("Reading directory..."); // Display once more
      readdirasked++;
      new_pad=0; // Erase command, to avoir manage cross twice a frame.
    }
    else
    {
      readdir(); // Read new dir
      readdirasked=0;
    }
    return;
  }

  // -- Do scrolling of name display (horizontal)
  if ( wantedothersnamelenght > currentothersnamelenght )
    currentothersnamelenght++;
  if ( wantedothersnamelenght < currentothersnamelenght )
    currentothersnamelenght--;

  // -- Check if current has screenshot and/or text for display lenght of others names
  wantedothersnamelenght=40;
  if ( files[selected].textname[0] != 0 )
  {
    wantedothersnamelenght=10;
  }
  else if ( files[selected].screenshotname[0] != 0 )
  {
    wantedothersnamelenght=29;
  }

  // -- Draw Names
  // Selected is id of selected files
  // Draw names from top (selected is on top), until end of page is reach, or 
  for (i=0;i<PERPAGE;i++)
  {
    char displayname[512];
    char displaynamefull[512];
    int current;

    current=i+selected;
    if (current>=filesnb)
      current-=filesnb;

    if ( (current==selected) && (i!=0) ) // When reach the selected line a second time then stops
      break;

    // -- Create "displaynamefull" (cut if too long)
    // Check if zip files path
    extractname(displayname,files[current].name, ext);

    if ( (files[current].flags & F_DIR) != 0 )
    { 
      strcpy(displaynamefull,displayname);
      sprintf(displayname,"[%s]",displaynamefull);
    }

    if ( current == selected )
    {
      if ( (files[selected].flags & F_DIR) == 0 )
      { 
        sprintf(displaynamefull,"%s (%s)", displayname, ext);
        strcpy(SelectedTitleExt, ext); // Store for display nfo game name (if any)
      }
      else
      { // Directory
        strcpy(displaynamefull, displayname);
      }
    }
    else
    {
      if ( strlen(displayname) <= currentothersnamelenght )
      {
        strcpy(displaynamefull,displayname);
      } else
      {
        strncpy(displaynamefull,displayname,currentothersnamelenght);
        displaynamefull[currentothersnamelenght]=0;
        strcat(displaynamefull, ".");
      }
    }

    // -- Display text info (if any)
    if (current == selected)
    {
      if ( files[current].textname[0] != 0 )
      {
        int j;

        gZipCurrentFileLocation=files[current].textposinzip; // To fasten research (-1 means that file is going to be searched in normal way)
        gZipCurrentNumFile=files[current].textzipnumfile;
        LoadAndFillTextInfoArray( files[current].textname );
        gZipCurrentFileLocation=-1; // unactive zip trick

        // Display Name of game and authors (first part of info text)
        for (j=0; j<8; j++)
        {
          render_text(NULL, WINX+2+currentothersnamelenght*8+8*2, WINY+4+CARAHEIGHT+j*CARAHEIGHT, SelectedTitleInfos[j], 0xF0F0F0);
        }
        // Display body part of info text
        // TODO, go to next page, or scrolling
        for (j=0; j<7; j++)
        {
          render_text(NULL, WINX+2+currentothersnamelenght*8+8*2, WINY+4+9*CARAHEIGHT+j*CARAHEIGHT+4, SelectedTitleInfos[8+j], 0xF0F0F0);
        }
      }
      else
      {
        // No info text files
        ResetNFOAutomaticInfosStored();
        //printf("No info text\n");
      }
    }

    // -- Display names 
    if ( (files[current].flags & F_DIR) == 0 )
    { 
      // Display File Name 
      if (current == selected)
      {
        // If current name is the selected one
        if ( SelectedTitleNameReadFromNFO[0]!=0)
          sprintf(displaynamefull, "%s (%s)",SelectedTitleNameReadFromNFO,SelectedTitleExt);

        // Test than filename is not too long.
        if ( strlen(displaynamefull) > WINW/8 )
          displaynamefull[(WINW/8)]=0; // Crop name to end of window

        render_text(NULL, WINX+2, WINY+4+i*CARAHEIGHT, displaynamefull, 0xF0F0F0);
      }
      else
      {
        render_text(NULL, WINX+2, WINY+4+i*CARAHEIGHT, displaynamefull, 0x30C070);
      }

      // -- Screenshot (if any)
      if (current == selected)
      {
        if ( files[current].screenshotname[0] != 0 )
        {
          PSPDisplayScreenshot( files[current].screenshotname, currentothersnamelenght );
        }
        else if ( SelectedTitleScreenshotFromNFO[0] != 0 )
        {
          int result;
          if ( waitbeforereadscreenshot == 0 ) // Screenshots in big zip take long time, so wait before loading screenshots to have fast parsing
          {
            result = PSPDisplayScreenshot( SelectedTitleScreenshotFromNFO, currentothersnamelenght );
            if (result==-1) // If screenshot not found
            {
              SelectedTitleScreenshotFromNFO[0]=0;
            }
          }else
          {
            waitbeforereadscreenshot--;
          }
        }
      }
    }
    else
    {
      // Display Directory name
      render_text(NULL, WINX+2, WINY+4+i*CARAHEIGHT, displaynamefull, 0xF0F0F0);
    }

  }

  // Green Border on first line
  PSPSetFillColor(0, 128, 0, 128);
  //PSPFillRectangle(WINX, WINY+2, WINX+WINW-160-4, WINY+4+8+2);
  PSPFillRectangle(WINX, WINY+2, WINX+WINW, WINY+4+8+2);

  // -- Update keys
  if (new_rel & PAD_SELECT) // Select = Exit
  {
    file_explorer_active = 0;
    menu_active = 0;
    is_paused=0;
  }
  if (new_pad & PAD_TRIANGLE) // Go back = Exit files brower
  {
    file_explorer_active = 0;
  }

  // Go up into directory list
  if ( (new_pad & PAD_CIRCLE) || (new_pad & PAD_SQUARE) ) // Go back = Exit files brower
  {
    go_upper();
    readdir_ask();
  }

  if (new_pad & PAD_CROSS) // Select a file or a directory
  {
    // Select files 

    // Apply automatic confi setting, like joystick port, if found into game
    if ( SelectedTitleJoystickPortFromNFO != -1 )
    {
      if ( SelectedTitleJoystickPortFromNFO == 1 )
        swap_joysticks=0;
      else
        swap_joysticks=1;
    }

    // Apply TDU if found in NDO
    if ( SelectedTitleTDUFromNFO == 1 )
    {
      // Force TDU true and set menu
      true_drive_emulation = 1;
      resources_set_value("DriveTrueEmulation", (resource_value_t)1);
    }

    // Directory is selected
    if (files[selected].flags & F_DIR)
    {
      if (!strcmp(files[selected].name, ".."))
        go_upper();
      else
        enter_dir(files[selected].name);

      readdir_ask();
    } 
    else
    {
      // File is selected
      callback((unsigned int) files[selected].name, callbackparam);
      file_explorer_active = 0;
    }

  }
  f = 0;
  if (new_pad & PAD_UP)
  {
    f = 1;
    repeatspeed=0;
  }
  else if (paddata & PAD_UP)
  {
    repeatcntr++;
    if ((repeatspeed>1&&repeatcntr>3)||repeatcntr>18)
      f = 1;
  }
  if (f)
  {
    repeatcntr = 0;
    repeatspeed++;
    selected--;
    if (selected < 0)
      selected = filesnb-1;
  }
  f = 0;
  if (new_pad & PAD_DOWN)
  {
    f = 1;
    repeatspeed=0;
  }
  else if (paddata & PAD_DOWN)
  {
    repeatcntr++;
    if ((repeatspeed>1&&repeatcntr>3)||repeatcntr>18)
      f = 1;
  }
  if (f)
  {
    repeatcntr = 0;
    repeatspeed++;
    selected++;
    if (selected >= filesnb)
      selected = 0;
  }

  f = 0;
  if (new_pad & PAD_LEFT)
  {
    f = 1;
    repeatspeed=0;
  } else if (paddata & PAD_LEFT)
  {
    repeatcntr++;
    if ((repeatspeed>1&&repeatcntr>3)||repeatcntr>18)
      f = 1;
  }
  if (f)
  {
    repeatcntr = 0;
    repeatspeed++;
    selected-=PERPAGE;
    if (selected < 0)
      selected = 0;
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
    if ((repeatspeed>1&&repeatcntr>3)||repeatcntr>18)
      f = 1;
  }

  if (f)
  {
    repeatcntr = 0;
    repeatspeed++;
    selected+=PERPAGE;
    if (selected >= filesnb)
      selected = filesnb - 1;
  }

  // -- Draw scroll bar
  draw_scrollbar();

}
