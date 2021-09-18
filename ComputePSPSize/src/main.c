// -----------------------------------------------------------------------------
// CK
// Compute PSP Size
// Compute the size needed to store files on a PSP
// Chunks of 32Kb or 64Kb
// -----------------------------------------------------------------------------

#include <direct.h> // Mkdir
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

char    gamespath[2048];
char    gamespathcurrent[2048];
#define SIZECLUSTER (32*1024)
int     nbcluster;

#define DESTMAINDIR "PSPGames\\"

// -----------------------------------------------------------------------------------

void processdir(char* dirname);

// -----------------------------------------------------------------------------------
void displayusage()
{
  printf("usage:\n");
  printf("exe path\\ \n");
}

// -----------------------------------------------------------------------------------
int main(int argc,char* argv[])
{
  char string[2048];
  HANDLE            hFile;
  WIN32_FIND_DATA   FindData;
  BOOL              bFindOK;

  if ( argc != 2 )
  {
    displayusage();
    exit(0);
  }

  nbcluster=0;

  // Get dirs
  strcpy(gamespath,argv[1]);

  strcpy(string,gamespath);
  strcat(string,"*.*");

  // Create result dir
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

        processdir(FindData.cFileName);
      }
        
      bFindOK = FindNextFile(hFile, &FindData);
    }
  }

  // Display results
  printf("All files use %d clusters of 32Kb, that means %d Kb\n", nbcluster, nbcluster*SIZECLUSTER );
}

// -----------------------------------------------------------------------------------
void processdir(char* dirname) 
{
  char string[2048];
  HANDLE            hFile,hFile2;
  WIN32_FIND_DATA   FindData,FindData2;
  BOOL              bFindOK,bFindOK2;
  FILE* file;

  // Parse all zip files
  strcpy(string,gamespathcurrent);
  strcat(string,"*.*");

  hFile   =   FindFirstFile(string,   &FindData);
  bFindOK = (INVALID_HANDLE_VALUE   !=   hFile);
  while ( bFindOK )
  {
    char  name_noext[256];
    char* pTmp;
    char  command[512];

    //printf("---- Processing %s ----\n", FindData.cFileName );
    int   filesize;
    FILE* file;

    strcpy(string,gamespathcurrent);
    strcat(string,FindData.cFileName);
    file=fopen(string,"r");
    if (file)
    {
      fseek(file,0,SEEK_END);
      filesize=ftell(file);
      if ( (filesize % SIZECLUSTER) == 0 )
      {
        nbcluster+=(filesize/SIZECLUSTER);
      }
      else
        nbcluster+=(filesize/SIZECLUSTER)+1;

      fclose(file);
    }

    bFindOK = FindNextFile(hFile,   &FindData);
  } // All zip files

}
