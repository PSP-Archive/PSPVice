#ifdef __cplusplus
extern "C"
{
#endif

#ifdef WIN32
void bzero(char* pBuffer, int size);
#endif
FILE* fopen_(const char *c1, const char *d, int test);
char mtoupper(char c);
int file_exist(const char *name);
#ifndef WIN32
FILE* fopen(const char *name, const char *mode);
#else
FILE* Myfopen(const char *name, const char *mode);
#endif
#ifndef WIN32
int fclose(FILE *f);
#else
int Myfclose(FILE *f);
#endif
#ifndef WIN32
int fflush(FILE *f);
#else
int Myfflush(FILE *f);
#endif
void rewind(FILE *f);
#ifndef WIN32
int fseek(FILE *f, long offset, int whence);
#else
int Myfseek(FILE *f, long offset, int whence);
#endif
char *fgets(char *s, int size, FILE *f);
int fendoffile(FILE *f);
int feof_(FILE *f);
#ifndef WIN32
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *f);
#else
size_t Myfread(void *ptr, size_t size, size_t nmemb, FILE *f);
#endif
#ifndef WIN32
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *f);
#else
size_t Myfwrite(const void *ptr, size_t size, size_t nmemb, FILE *f);
#endif
#ifndef WIN32
int fputc(int c, FILE *f);
#else
int Myfputc(int c, FILE *f);
#endif
int fputs(const char *s, FILE *f);
#ifndef WIN32
int fgetc(FILE *f);
#else
int Myfgetc(FILE *f);
#endif

#ifdef __cplusplus
};
#endif