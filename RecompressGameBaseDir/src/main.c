// -----------------------------------------------------------------------------
// CK
// Rcompress gamebase directories
//
// Run this inti a game subdir
// Result will be out.zip
// -----------------------------------------------------------------------------

#include <direct.h> // Mkdir
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

char gamespath[2048];
char gamespathcurrent[2048];
char destpath[2048];
char screenshotspath[2048];

#define DESTMAINDIR "PSPGames\\"

// -----------------------------------------------------------------------------------

void processdir(char* dirname);

// -----------------------------------------------------------------------------------
void displayusage()
{
  printf("This tool recompress GameBase V05 directory to fit PSPVice format\n");
  printf("The result will be into a dir named 'PSPGames', same place as exe\n");
  printf("Copy content to the psp, into dir : __SCE__PSPVice/games/\n");
  printf("\n");
  printf("usage:\n");
  printf("exe pathofgames\\ pathofscreenshots\\ \n");
  printf(" pathofgames: Path where all the game directories are : 0 a1 a2 b1 ...\n");
  printf(" pathofscreenshots: Path where all the screenshots directories are : 0 A B C D ...\n");
  printf("additional exe must be present : 7z.exe\n");
}

// -----------------------------------------------------------------------------------
int main(int argc,char* argv[])
{
  char string[2048];
  HANDLE            hFile;
  WIN32_FIND_DATA   FindData;
  BOOL              bFindOK;

  if ( argc != 3 )
  {
    displayusage();
    exit(0);
  }

  // Get dirs
  strcpy(gamespath,argv[1]);
  strcpy(screenshotspath,argv[2]);

  // Parse all directories
  strcpy(string, gamespath);
  strcat(string, "*.*");

  // Create result dir
  mkdir(DESTMAINDIR);

  hFile   =   FindFirstFile(string,   &FindData);
  bFindOK = (INVALID_HANDLE_VALUE   !=   hFile);
  while ( bFindOK )
  {
    if ( FindData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY )
    {
      if ( stricmp(FindData.cFileName,".") != 0 && stricmp(FindData.cFileName,"..")!=0 )
      {
        printf("----------------------------------------------\n");    
        printf("Parsing dir %s\n", FindData.cFileName);

        // Copy source dir to string
        strcpy(gamespathcurrent, gamespath);
        strcat(gamespathcurrent, FindData.cFileName);
        strcat(gamespathcurrent, "\\");

        // Create dest dir
        strcpy(destpath, DESTMAINDIR);
        strcat(destpath, FindData.cFileName);
        mkdir(destpath);
        strcat(destpath, "\\");

        processdir(FindData.cFileName);
      }
        
      bFindOK = FindNextFile(hFile, &FindData);
    }
  }

  system("rmdir /S /Q tempgames");
  system("rmdir /S /Q tempgamesall");
  system("rmdir /S /Q tempscreens");
}

// -----------------------------------------------------------------------------------
void processdir(char* dirname) 
{
  char string[2048];
  char nfofile[10240]; // Read NFO. no NFO is bigger than 10240 bytes
  char screenshotname[512];
  HANDLE            hFile,hFile2;
  WIN32_FIND_DATA   FindData,FindData2;
  BOOL              bFindOK,bFindOK2;
  FILE* file;

  // Copy all files to a tempgames dir
  // Copy all screenshots to a tempscreens dir
  mkdir("tempgames"); // current game only
  mkdir("tempgamesall"); // All unzipped games
  mkdir("tempscreens");
  system("del /F /Q tempgames\\*.*");
  system("del /F /Q tempgamesall\\*.*");
  system("del /F /Q tempscreens\\*.*");

  // Parse all zip files
  strcpy(string,gamespathcurrent);
  strcat(string,"*.zip");

  hFile   =   FindFirstFile(string,   &FindData);
  bFindOK = (INVALID_HANDLE_VALUE   !=   hFile);
  while ( bFindOK )
  {
    char  name_noext[256];
    char* pTmp;
    char  command[512];

    printf("---- Processing %s ----\n", FindData.cFileName );

    // Unzip (pkunzip only support short names)
    sprintf(command, "copy %s%s tempgames\\1.zip", gamespathcurrent, FindData.cFileName);
    system(command);

    // For all files, copy NFO
    _chdir("tempgames");
    system("..\\7z.exe e 1.zip");
    system("del 1.zip");
    {
      hFile2   =   FindFirstFile("*.*",   &FindData2);
      bFindOK2 = (INVALID_HANDLE_VALUE   !=   hFile2);
      while ( bFindOK2 )
      {
        // Copy Version.nfo to filename of all files
        if ( stricmp( FindData2.cFileName, "version.nfo") != 0 && !(FindData2.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) )
        {
          char  namefile_noext[256];
          // Get name with no ext
          strcpy(namefile_noext,FindData2.cFileName);
          pTmp=strrchr(namefile_noext, '.');
          if (pTmp)
            pTmp[0]=0;
          else
            continue; // No extension on name

          // Copy nfo
          // Unzip to dir
          sprintf(command, "copy Version.nfo %s.nfo", namefile_noext);
          system(command);
        }
        bFindOK2 = FindNextFile(hFile2, &FindData2);
      }
      // -- Load NFO and get screenshot name
      screenshotname[0]=0;
      file=fopen("version.nfo","rb");
      if (file)
      {
        int filesize;
        int position=0;

        filesize=fread(nfofile, 1, 10240, file);
        fclose(file);
        
        // Search for string "Screenshot:"
        while ( position<filesize && strnicmp(&nfofile[position], "Screenshot:", 11) != 0 )
        {
          position++;
        }
        if ( position<filesize ) // We found it
        {
          position += 11; // Go over "Screenshot:";
          while ( (nfofile[position] == ' ' || nfofile[position] == '\t') && position<filesize )
          {
            position++;
          }
          if ( position<filesize ) // We found the name
          {
            int namepos=0;
            while( nfofile[position] != 0x0d )
            {
              screenshotname[namepos]=nfofile[position];
              namepos++;
              position++;
            }
            screenshotname[namepos]=0;
          }
        }
      } // Get screenshot name

      // All nfo are copied
      system("del version.nfo");
    }
    _chdir("..");
    system("move tempgames\\*.* tempgamesall\\");

    // Copy screenshot if any
    if ( screenshotname[0] != 0 )
    {
      sprintf(string, "copy \"%s%s\"  tempscreens",screenshotspath,screenshotname);
      system(string);
    }

    bFindOK = FindNextFile(hFile,   &FindData);
  } // All zip files

  // All files and screenshots are copied
  // Create the zip
  // Command is pkzip destzip sourcefiles
  chdir("tempgamesall");
  sprintf(string, "..\\7z.exe a -tzip ..\\%s%s\\%s.zip *.* -mx9",DESTMAINDIR,dirname,dirname);
  system(string);
  chdir("..");

  chdir("tempscreens");
  sprintf(string, "..\\7z.exe a -tzip ..\\%s%s\\Screenshots.zip *.* -mx9",DESTMAINDIR,dirname);
  system(string);
  chdir("..");

  system("del /F /Q tempgames\\*.*");
  system("del /F /Q tempgamesall\\*.*");
  system("del /F /Q tempscreens\\*.*");
}
