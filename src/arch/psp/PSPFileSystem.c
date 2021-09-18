#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unzip.h"
#include "malloc.h"

#ifndef WIN32
#include <pspkernel.h> 
#endif
#include "PSPSpecific.h"
#include "pspmain.h"

#ifdef WIN32
#undef fopen
#undef fread
#undef fwrite
#undef ftell
#undef fflufh
#undef fclose
#undef fseek
#undef fputc
#undef fgetc
#endif

// Redefine content of FILE structure for WIN32 debug (not same names on PSP SDK)
#ifdef WIN32
#define _ur       _charbuf  // Store id of temps file (-1=No temp file ... else temp file is used 1 2 3 4 ... 99 = Parsing zip file)
#define _p        _ptr      // Pointer to buffer
#define _flags    _flag     // Flags
#define _offset   _cnt      // Current position in file
#define _r        _bufsiz   // Size of total file
// _file is used by both.

FILE* gCurrentZipFile; // Current opened zip file
#endif

FILE *tempfile[4] = {NULL, NULL, NULL, NULL}; 

// Buffer to read quickly zip file (beacause zip is accessed byte by byte)
//#define READBUFFERSIZE 1024
#define READBUFFERSIZE 2048
//#define READBUFFERSIZE 16 // Debug
char    gReadBuffer[READBUFFERSIZE];
int     gReadBufferPosition=-1;        // -1 means no buffer. >0 give position in file
int     gReadBufferCurrentPosition=-1; // 0 to READBUFFERSIZE

// Fast Zip reading (remove the unZLocateFile step, which use lot of seek and read)
int     gZipCurrentFileLocation=-1; // -1=No location stored (will store one), >0 = Location store, use it
int     gZipCurrentNumFile=0; // Internal zip also need "numfile" info

#ifdef WIN32
void bzero(char* pBuffer, int size)
{
  memset((void*)pBuffer,0,size);
}
#endif

#ifdef WIN32
int gTotalSeek=0;
int gTotalRead=0;
int gTotalReadBuffer=0;
#endif

// --------------------------------------------------------------------------
// Name : fopen_
// File name can start with ms0:... if an absolute path+file is asked
// File name can start with the C64 system directories c64sys or drivesys
// File name can start wil z: if file is contained into a zip file. z:zipname/filename
// If none above, then it is a file to open into ms0:/PSP/Game/PSPVICE/games/
// FOpen reads the whole file into a buffer and close the file.
// All operations are done into that buffer then (fread)
// --------------------------------------------------------------------------
FILE* fopen_(const char *c1, const char *d, int test)
{
  char fullname[2048];
  char fullname1[2048];
  FILE* mynew;

  int     byteread;
  int     i,j;
  char    zipname[256];
#ifdef WIN32
  // Win32
  FILE*   realfile;
#else
  SceUID  fid;
#endif

  printf("Fopen File %s (mode %s)\n", c1,d);

  // -- Build final name
  // -- Check for c64 system directories
  if ( strncmp( c1, "/c64sys", 7) == 0 ) // If system file
  {
    strcpy(fullname1, path_root);
    strcat(fullname1, "/PSPViceDatas");
    strcat(fullname1, c1);
  }
#ifdef PSPVIC20
  else if ( strncmp( c1, "/vic20sys", 7) == 0 ) // If system file
  {
    strcpy(fullname1, path_root);
    strcat(fullname1, "/PSPViceDatas");
    strcat(fullname1, c1);
  }
#endif
#ifdef PSPPLUS4
  else if ( strncmp( c1, "/plus4sys", 7) == 0 ) // If system file
  {
    strcpy(fullname1, path_root);
    strcat(fullname1, "/PSPViceDatas");
    strcat(fullname1, c1);
  }
#endif
  else if ( strncmp( c1, "/drivesys", 9) == 0 ) // If system file
  {
    strcpy(fullname1, path_root);
    strcat(fullname1, "/PSPViceDatas");
    strcat(fullname1, c1);
  }
  /*
  else if ( stricmp( c1, "screenshots.zip") == 0 ) // If system file
  {
    strcpy(fullname1, path_root);
    strcat(fullname1, "/games/");
    strcat(fullname1, c1);
  }
  */
  else
  {
    // If no system directories then put games path
    strcpy(fullname1, path_games);
    strcat(fullname1, c1);
  }

  // -- If a full path was already there, then let him
  if ( strncmp( c1, "ms0", 3) == 0 )
  {
    strcpy(fullname1, c1);
  }

  printf("Fopen File fullname %s\n", fullname1);

  // -- If file is inside a zip file -------------------------
  // name is something like "z:myzip.zip/myfile.D64"
  // Then uncompresse file
  if ( strncmp( c1, "z:", 2) == 0 )
  {
    char* pTmp;
    int namesize;
    unzFile fd;
    unz_file_info info;
    int ret = 0;
    int filesize;
    printf("fopen_ : Zipped file opening asked %s\n", c1);
    if (test)
    {
      printf("File in test mode ... Test OK ... exiting");
      return (FILE*)1;
    }
    // -- Construct Zip name
    pTmp=(char*)strchr(c1, '/'); // Search first / position
    if (pTmp==NULL)
      return NULL;
    namesize = pTmp-(c1+2);
    strncpy(zipname, c1+2, namesize );
    zipname[namesize]=0;
    // -- Construct file name
    strcpy(fullname,pTmp+1);
    printf("Fopen: Zip name %s, filename %s\n", zipname, fullname);
    // Open file
    // Attempt to open the archive
    fd = unzOpen(zipname);
    if(!fd)
    {
      printf("Fopen: Can not open Zip file %s\n",zipname);
      return NULL;
    }
    // Locate file.
    // Optimised version. First time the zip is parse, store the location
    // Use it the others time
    if ( gZipCurrentFileLocation == -1 )
    {
      ret = unzLocateFile(fd,fullname,2); // No case sensitive
      //gZipCurrentFileLocation=unzGetFilePosInDir(fd); // These info are get into ParseZip
      //gZipCurrentNumFile=unzGetNumFile(fd);
    }
    else
    {
      unzSetFilePosInDir(fd,gZipCurrentFileLocation);
      unzSetNumFile(fd,gZipCurrentNumFile);
    }
    if ( ret != UNZ_OK)
    {
      printf("Fopen: Can not locate file %s\n",fullname);
      unzClose(fd);
      return NULL;
    }
    ret = unzGetCurrentFileInfo(fd, &info, NULL, 0, NULL, 0, NULL, 0);
    if ( ret != UNZ_OK)
    {
      printf("Fopen: Can not read file info\n");
      unzClose(fd);
      return NULL;
    }
    filesize = info.uncompressed_size;
    // Allocate space for reading
    mynew = (FILE*) malloc(sizeof(FILE));
    bzero((char*)mynew, sizeof(FILE));
    mynew->_ur = -1;
    mynew->_flags = 0;
    mynew->_file = 0; // TODO : file if of file, what to write here ??
    mynew->_r = filesize; 
    mynew->_p = (char*) malloc(mynew->_r + 1);
    mynew->_offset = 0;
    printf("Uncompressed size %d\n", filesize);
    // Uncompress file
    ret = unzOpenCurrentFile(fd);
    if (ret != UNZ_OK )
    {
      printf("Can not open current file (%d)\n", ret);
      free(mynew->_p);
      free(mynew);
      unzClose(fd);
      return NULL;
    }
    ret = unzReadCurrentFile(fd,mynew->_p,mynew->_r);
    if (ret < 0 )
    {
      printf("Error in decompressing file (%d)\n", ret);
      free(mynew->_p);
      free(mynew);
      unzClose(fd);
      unzCloseCurrentFile(fd);
      return NULL;
    } 
    unzCloseCurrentFile(fd);
    // Return
    unzClose(fd);
    return mynew;
  } // ------------------------------ end zip file -------------------

  // -- Remove double //
  i=0;
  j=0;
  while(fullname1[i])
  {
    if ( !(fullname1[i]=='/' && fullname1[i+1]=='/' ) )
    {
      fullname[j]=fullname1[i];
      j++;
    }
    i++;
  }
  fullname[j]=0;

  //printf("Fopen_ %s\n", fullname);

  mynew = (FILE*) malloc(sizeof(FILE));
  bzero((char*)mynew, sizeof(FILE));

  //printf ("opening '%s' '%s'\n", c, d);
  //if (!strncmp(c, "cdfs:", 5) || !strncmp(c, "cdrom0:", 7)) {
  //  cdvd_access();
  //}

  mynew->_ur = -1;   

  if (!strncmp(c1, "temp", 4))
  {
    mynew->_ur = atoi(c1+4);
  }

  if (  ( (mynew->_ur < -1) || (mynew->_ur > 4) ) )
  {
    free(mynew);
    return NULL;
  }

  mynew->_flags = 0;

  // -- File open in write mode ------------------------------
  //if (index(d, 'w')!=NULL || index(d, '+')!=NULL)
  if ( (stricmp(d, "w")==0) || (stricmp(d, "wb")==0) )
  {
    printf("Open in write mode\n");
    if (mynew->_ur==-1)
    {
#ifndef WIN32
      mynew->_file = sceIoOpen(fullname, PSP_O_WRONLY | PSP_O_CREAT, 0777); 
#else
      realfile = fopen( fullname, "wb");
#endif
    }
    mynew->_flags |= 1;
    mynew->_r = 0;
    mynew->_offset = 0;
    mynew->_p = (char*) malloc(mynew->_r + 1);
    if (mynew->_ur!=-1)
    {
      if (tempfile[mynew->_ur]!=NULL)
      {
        free(tempfile[mynew->_ur]->_p);
        free(tempfile[mynew->_ur]);
      }
      tempfile[mynew->_ur]=mynew;
    }
    return mynew;
  }
  else
  {
    // -- Open file in read mode

    if (mynew->_ur!=-1)
    {
      free(mynew);
      if (tempfile[mynew->_ur]==NULL)
      {
        return NULL;
      }
      tempfile[mynew->_ur]->_offset = 0;
      tempfile[mynew->_ur]->_flags = 0;
      return tempfile[mynew->_ur];
    }

#ifndef WIN32
    fid = sceIoOpen(fullname, PSP_O_RDWR /*PSP_O_RDONLY*/, 0); // CK : Read and write, user can save on C64 disk
    if (fid<0)
      mynew->_file = -1;
    else
      mynew->_file = fid;
#else
    realfile = fopen(fullname, "rb");
    if (realfile==NULL)
      mynew->_file = -1;
    else
      mynew->_file = 0; // means ok
#endif

    //printf("Open return %d", fid);
    // Pb !!! sceIoOpen return 2 when file do not exists (should return >0)
  }

  if (mynew->_file<0)
  {
    printf("File not found exiting\n");
    free(mynew);
    return NULL;
  }

  if (test)
  {
    printf("File in test mode ... Test OK ... exiting");
#ifndef WIN32
    sceIoClose(mynew->_file); 
#else
    fclose(realfile);
#endif
    free(mynew);
    return (FILE*)1;
  }

  // -- Compute size
#ifndef WIN32
  mynew->_r = sceIoLseek(mynew->_file, 0, SEEK_END); 
  sceIoLseek(mynew->_file, 0, SEEK_SET); 
#else
  fseek(realfile, 0, SEEK_END);
  mynew->_r = ftell(realfile);
  fseek(realfile, 0, SEEK_SET);
#endif

  // -- If file is a BIGGGG ZIP file, then do not open it fully (or BIG archive will fail)
  if ( mynew->_r > 500000 ) // Zip bigger than 500Kb are not buffered into memory
  {
#ifdef WIN32
  gCurrentZipFile=NULL; 
#endif
  {
  char* pTmp;
  pTmp = strrchr(fullname, '.');
  if (pTmp && stricmp(pTmp,".zip")==0 )
  {
#ifndef WIN32
    mynew->_ur = 99; // That means we are parsing a file file, do not store into memory
    printf("Fopen: .zip file detected, opening file without buffering\n");
    gReadBufferPosition=-1;
    gReadBufferCurrentPosition=0;
    return mynew;   // Stop here, we do not open the file
#else
    mynew->_ur = 99; // That means we are parsing a file file, do not store into memory
    gCurrentZipFile=realfile;
    gReadBufferPosition=-1;
    gReadBufferCurrentPosition=0;
    return mynew;   // Stop here, we do not open the file
#endif
  }
  }
  } // If size > 500Kb

  // Alloc space for storing whole file
  mynew->_p = (char*) malloc(mynew->_r + 1);
  mynew->_offset = 0;

  printf("Size of file %d, file adress %08x\n", mynew->_r, mynew);

  // Check error (negative size or not enough memory to load file)
  if (mynew->_r < 0 || mynew->_p == NULL)
  {
#ifndef WIN32
    sceIoClose(mynew->_file); 
#else
    fclose(realfile);
#endif
    if (mynew->_p != NULL)
      free(mynew->_p);
    free(mynew);
    return NULL;
  }

  // -- Read datas
  if (mynew->_r)
  {
#ifndef WIN32
    byteread = sceIoRead(mynew->_file, mynew->_p, mynew->_r);
#else
    byteread = fread(mynew->_p, 1, mynew->_r,realfile);
#endif

    //printf("Byte read = %d", byteread);

    if (byteread!=mynew->_r)
    {
#ifndef WIN32
      sceIoClose(mynew->_file); 
#else
      fclose(realfile);
#endif
      free(mynew->_p);
      free(mynew);
      return NULL;
    }
  }

  // CK : Do not close file here. All file can potentially be written (if user make a save to disk or tape)
  // File must be close manually.
  //if (!(mynew->_flags & 1))
  //  sceIoClose(mynew->_file); 

  //printf("Fopen exiting");

#ifdef WIN32 // No save for debug on WIN32
  fclose(realfile);
#endif

  return mynew;

  // TODO CK
}

// --------------------------------------------------------------------------
// Name : 
// --------------------------------------------------------------------------
char mtoupper(char c) {
  if (c>='a' && c<='z')
    return c+'A'-'a';
  return c;
}

// --------------------------------------------------------------------------
// Name : 
// --------------------------------------------------------------------------
int file_exist(const char *name)
{
  int i=0;
  char fn[128];

  char upper[128];
  char upper2[128];
  char lower[128];
  char allupper[128];
  char alllower[128];

#ifdef PSPDEBUG
  printf ("testing '%s'", name);
#endif

  FILE* file;

#ifndef WIN32
  snprintf (fn, sizeof(fn), "%s", name);
  file=fopen_(fn, "r", 1);
  if (file!=NULL)
  {
    fclose(file);
    return 1;
  }
#else
  file=fopen_(name, "r", 1); // snprintf not standard on WIN32
  if (file!=NULL)
  {
    fclose(file);
    return 1;
  }
#endif

  return 0;
}

// ---------------------------------------------------------------
// Name : 
// ---------------------------------------------------------------
#ifndef WIN32
FILE* fopen(const char *name, const char *mode)
#else
FILE* Myfopen(const char *name, const char *mode)
#endif
{
  int i=0;
  FILE *f;
  char fn[128];
  char upper[128];
  char upper2[128];
  char lower[128];
  char allupper[128];
  char alllower[128];

#ifdef WIN32
 gTotalSeek=0;
 gTotalRead=0;
 gTotalReadBuffer=0;
#endif

#ifdef PSPDEBUG
  printf("fopen starts");
#endif
  
  f=fopen_(name, mode, 0);

  return f; // f NULL if error
}

int numwr = 0;

// ---------------------------------------------------------------
// Name : 
// ---------------------------------------------------------------
#ifndef WIN32
int fclose(FILE *f)
#else
int Myfclose(FILE *f)
#endif
{

#ifdef WIN32
printf("Close file. Total Seek %d Total Read %d, ReadBuffer %d\n", gTotalSeek, gTotalRead,gTotalReadBuffer);
#endif

  numwr = 0;
  //printf ("close %d (%s)%s\n", f->_file, f->_flags&1 ? "w" : "r", f->_ur!=-1 ? " (tempfile)" : "");
#ifdef PSPDEBUG
  printf("PSP fclose (file = %08x)\n",f);
#endif
  if (f->_ur!=-1 && f->_ur!=99)
  {
    return 0;
  }

#ifndef WIN32
  if (f->_flags & 1) // Write
  {
#ifdef PSPDEBUG
    printf("PSP fclose, writing file (file = %08x) Size=%d\n",f,f->_r);
#endif
    sceIoLseek32(f->_file, 0, SEEK_SET);
    sceIoWrite(f->_file, f->_p, f->_r);
  }
#endif

#ifndef WIN32
  sceIoClose(f->_file); 
#else
  if ( f->_ur==99 )
    fclose(gCurrentZipFile);
#endif

  if ( f->_ur!=99 ) // No buffer for parsing zip file
    free(f->_p);
  free(f);
  return 0;
}

// ---------------------------------------------------------------
// Name : 
// CK : Empty because happend at each fwrite
// ---------------------------------------------------------------
#ifndef WIN32
int fflush(FILE *f)
#else
int Myfflush(FILE *f)
#endif
{
  /*
  #ifdef PSPDEBUG
  printf("PSP fflush (file = %08x)\n",f);
  #endif
  if (f == NULL)
  return 0;

  if (f->_flags & 1 ) //Write
  {
  sceIoLseek(f->_file,0,SEEK_SET);
  sceIoWrite(f->_file,f->_p,f->_r);
  }
  */
  return 0;
}

// ---------------------------------------------------------------
// Name : 
// ---------------------------------------------------------------
#ifndef WIN32
long ftell(FILE *f)
#else
long Myftell(FILE *f)
#endif
{
  if ( f->_ur==99 )
  {

    // If buffer is used
    if (gReadBufferPosition != -1 )
    {
#ifndef WIN32
      return sceIoLseek(f->_file, gReadBufferPosition+gReadBufferCurrentPosition, SEEK_SET); 
#else
      fseek(gCurrentZipFile, gReadBufferPosition+gReadBufferCurrentPosition, SEEK_SET );
      return ftell(gCurrentZipFile);
#endif
    }

#ifndef WIN32
  return sceIoLseek(f->_file, 0, SEEK_CUR); 
#else
  return ftell(gCurrentZipFile);
#endif
  }

  return f->_offset;
}

// ---------------------------------------------------------------
// Name : 
// ---------------------------------------------------------------
void rewind(FILE *f)
{
  f->_offset = 0;
}

// ---------------------------------------------------------------
// Name : fseek
// If successful, the function returns a zero value.
// ---------------------------------------------------------------
#ifndef WIN32
int fseek(FILE *f, long offset, int whence)
#else
int Myfseek(FILE *f, long offset, int whence)
#endif
{

#ifdef WIN32
 gTotalSeek++;
#endif

  if ( f->_ur==99 )
  {
    // Zip only use set fseek.
    // If seek is inside buffer then do not do anything
    // TODO Debug this !!
    /*
    if ( gReadBufferPosition!=-1 &&  (offset>=gReadBufferPosition) && (offset<gReadBufferPosition+READBUFFERSIZE) )
    {
      // Change position in buffer
      if ( whence != SEEK_SET )
      {
        printf("Aie");
      }
      gReadBufferCurrentPosition = offset-gReadBufferPosition;
      return 0;
    }
    */
    

#ifndef WIN32
  sceIoLseek(f->_file,offset,whence);
  gReadBufferPosition=-1; // Invalidate buffer
  return 0; // means no error (to know position, use ftell)
#else
  //printf("Seek %d %d\n", offset, whence);
  gReadBufferPosition=-1; // Invalidate buffer
  return fseek(gCurrentZipFile,offset,whence);
#endif
  }

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

#ifdef PSPDEBUG
  //printf ("seek: offset = %d  r = %d\n", f->_offset, f->_r);
#endif

  if (f->_offset < 0 || f->_offset > f->_r) {
    f->_offset = f->_r;
    return -1;
  }

  return 0;
}

// ---------------------------------------------------------------
// Name : 
// ---------------------------------------------------------------
char *fgets(char *s, int size, FILE *f)
{
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

// ---------------------------------------------------------------
// Name : 
// ---------------------------------------------------------------
int fendoffile(FILE *f)
{
#ifdef PSPDEBUG
  printf("Eof Pos=%d Size=%d\n", f->_offset,  f->_r);
#endif
  if (f->_offset >= f->_r)
    return 1;
  return 0;
}

// ---------------------------------------------------------------
// Name : 
// ---------------------------------------------------------------
int feof_(FILE *f)
{
#ifdef PSPDEBUG
  printf("Eof Pos=%d Size=%d\n", f->_offset,  f->_r);
#endif
  if (f->_offset >= f->_r)
    return 1;
  return 0;
}

// ---------------------------------------------------------------
// Name : 
// Input : Size of one element, number of elements
// return : Number of elements read (NOT SAME AS NUMBER OF BYTES !)
// ---------------------------------------------------------------
#ifndef WIN32
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *f)
#else
size_t Myfread(void *ptr, size_t size, size_t nmemb, FILE *f)
#endif
{
  int i;
  char* dest;
  int byteread;

#ifdef WIN32
 gTotalRead++;
#endif

  if ( f->_ur==99 )
  {
    if ( size*nmemb > READBUFFERSIZE ) // Big piece, do not use buffer
    {
      // Set real position in file
      if ( gReadBufferPosition != -1 )
      {
#ifndef WIN32
      ftell(f);
#else
      Myftell(f);
#endif
      }
      gReadBufferPosition=-1; // Invalidate buffer
  #ifndef WIN32
      byteread=sceIoRead(f->_file, ptr, size*nmemb);
      if (byteread==size*nmemb)
        return nmemb;
      else
        return 0;
  #else
      return fread(ptr,size,nmemb, gCurrentZipFile);
  #endif
    }
    // Small piece and no buffer, so FILL buffer
    if ( gReadBufferPosition == -1 || (gReadBufferCurrentPosition+size*nmemb)>READBUFFERSIZE )
    {
  #ifndef WIN32
      gReadBufferPosition=ftell(f);
#else
      gReadBufferPosition=Myftell(f);
#endif
      gReadBufferCurrentPosition=0;
      //printf("Fill buffer position\n", size*nmemb);
  #ifndef WIN32
      sceIoRead(f->_file, gReadBuffer, READBUFFERSIZE);
  #else
      fread(gReadBuffer,1,READBUFFERSIZE,gCurrentZipFile);
  #endif
    }
    // Copy bytes from buffer
    //printf("Read (buffer)%d\n", size*nmemb);
    dest=(char*)ptr;
    for (i=0; i<size*nmemb; i++)
    {
      dest[i]=gReadBuffer[gReadBufferCurrentPosition];
      gReadBufferCurrentPosition++;
    }
#ifdef WIN32
 gTotalReadBuffer++;
#endif
    return nmemb;
  }

  for (i=0;i<nmemb;i++) {
    if (f->_offset + size > f->_r)
      break;
    memcpy(ptr, &f->_p[f->_offset], size);
    //ptr += size;
    ptr = (void*) (((size_t)ptr) + size);
    f->_offset += size;
  }
  return i;
}

// ---------------------------------------------------------------
// Name : fwrite
// The datas are written into memory (and not physically to disk)
// When closing the file, the whole file will be written to PSP memory stick
// ---------------------------------------------------------------
#ifndef WIN32
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *f)
#else
size_t Myfwrite(const void *ptr, size_t size, size_t nmemb, FILE *f)
#endif
{
  int len = size*nmemb, i;
#ifdef PSPDEBUG
  printf ("PSP fwrite %d %d (%d)\n", size, nmemb, len);  
#endif

  // CK : Since the file have been accessed with command fwrite, tag it as "write" file,
  // for fclose will write to memory stick the real physical file
  f->_flags |= 1;

  if (f->_offset+len > f->_r)
  {
#ifndef WIN32
    f->_p = (unsigned char*) realloc(f->_p, f->_offset+len);
#else
    f->_p = (char*) realloc(f->_p, f->_offset+len);
#endif
    f->_r = f->_offset+len;
  }
  for (i=0;i<len;i++)
  {
    f->_p[f->_offset++] = ((unsigned char*)ptr)[i];
  }
  numwr += len;
  //  printf ("numwr = %d  offset = %d  r = %d\n", numwr, f->_offset, f->_r);
  return nmemb;
}

// ---------------------------------------------------------------
// Name : 
// ---------------------------------------------------------------
#ifndef WIN32
int fputc(int c, FILE *f)
#else
int Myfputc(int c, FILE *f)
#endif
{
  if (f == stdout) {
    printf ("%c", c);
    return (unsigned char)c;
  }
  if (f->_offset >= f->_r)
  {
#ifndef WIN32
    f->_p = (unsigned char*) realloc(f->_p, f->_r+1);
#else
    f->_p = (char*) realloc(f->_p, f->_r+1);
#endif
    f->_r++;
  }
  f->_p[f->_offset++] = c;
  //  printf ("fputc %d %d\n", c, f->_file);
  //  printf ("numwr = %d  offset = %d  r = %d\n", ++numwr, f->_offset, f->_r);
  return (unsigned char)c;
}

// ---------------------------------------------------------------
// Name : 
// ---------------------------------------------------------------
int fputs(const char *s, FILE *f)
{
  int len = strlen(s), i;
  if (f == stdout) {
    printf ("%s", s);
    return len;
  }
  if (f->_offset+len > f->_r)
  {
#ifndef WIN32
    f->_p = (unsigned char*) realloc(f->_p, f->_offset+len);
#else
    f->_p = (char*) realloc(f->_p, f->_offset+len);
#endif
    f->_r = f->_offset+len;
  }
  for (i=0;i<len;i++) {
    f->_p[f->_offset++] = s[i];
  }
  return len;
}

// ---------------------------------------------------------------
// Name : 
// ---------------------------------------------------------------
#ifndef WIN32
int fgetc(FILE *f)
#else
int Myfgetc(FILE *f)
#endif
{
  if (f->_offset >= f->_r)
    return -1;
  return f->_p[f->_offset++];
}
