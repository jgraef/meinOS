#ifndef _STDIO_H_
#define _STDIO_H_

#include <sys/types.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define BUFSIZ 2*1024

#define _IOLBF

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define EOF -1

#ifndef NULL
  #define NULL ((void*)0)
#endif

#define L_tmpnam PATH_MAX
#define P_tmpdir "/tmp"

typedef struct {
  int fh;
  int error;
  int mode;
  int eof;
  void *buf;
  void *bufcur;
  int dobuf;
} FILE;

typedef off_t fpos_t;

FILE *stdin;
FILE *stdout;
FILE *stderr;

void clearerr(FILE *file);
int feof(FILE *stream);
int ferror(FILE *stream);
int fclose(FILE *stream);
FILE *fdopen(int fh,const char *mode);
int fileno(FILE *stream);
FILE *fopen(const char *name,const char *mode);
int fflush(FILE *stream);

int rename(const char *old, const char *new);

int puts(const char *s);
size_t _fwrite(const void *ptr,size_t size,FILE *stream);

char *fgets(char *s,int n,FILE *stream);
size_t _fread(void *ptr,size_t size,FILE *stream);

int printf(const char * format, ...);
int sprintf(char * buffer, const char * format, ...);
int snprintf(char * buffer, size_t size, const char * format, ...);
int fprintf(FILE * fp, const char * format, ...);
int asprintf(char ** buffer, const char * format, ...);

int vprintf(const char * format, va_list);
int vsprintf(char * buffer, const char * format, va_list);
int vsnprintf(char * buffer, size_t size, const char * format, va_list);
int vfprintf(FILE * fp, const char * format, va_list);
int vasprintf(char ** buffer, const char * format, va_list);

int sscanf(const char *str,const char *format,...);

int vsscanf(const char *buffer,const char *format,va_list ap);



static inline int remove(const char *path) {
  return unlink(path);
}

/**
 * Puts a char on stream
 *  @param c Character
 *  @param stream Stream
 *  @return Char written
 */
static inline int fputc(int c,FILE *stream) {
  char chr = c;
  return _fwrite(&chr,1,stream)==1?c:EOF;
}

/**
 * Puts a string on a stream
 *  @param s String to put on stream
 *  @param stream Stream
 *  @return How many bytes written
 */
static inline int fputs(const char *s,FILE *stream) {
  return _fwrite(s,strlen(s),stream);
}

/**
 * Writes binary
 *  @param ptr Buffer to get data from
 *  @param size Size of each element
 *  @param nitems How many items to write
 *  @param stream Stream to write to
 *  @return How many elements written
 */
static inline size_t fwrite(const void *ptr,size_t size,size_t nelem,FILE *stream) {
  return _fwrite(ptr,size*nelem,stream);
}

/**
 * Seeks in stream
 *  @param stream Stream
 *  @param offset Offset
 *  @param whence Whence
 *  @return Success?
 */
static inline int fseeko(FILE *stream,off_t offset,int whence) {
  fflush(stream);
  return lseek(stream->fh,offset,whence)==offset?0:-1;
}

/**
 * Seeks in stream
 *  @param stream Stream
 *  @param offset Offset
 *  @param whence Whence
 *  @return Success?
 */
static inline int fseek(FILE *stream,long offset,int whence) {
  return fseeko(stream,(off_t)offset,whence);
}

/**
 * Returns file offset in stream
 *  @param stream Stream
 *  @return Offset
 */
static inline off_t ftello(FILE *stream) {
  return lseek(stream->fh,0,SEEK_CUR);
}

/**
 * Returns file offset in stream
 *  @param stream Stream
 *  @return Offset
 */
static inline long ftell(FILE *stream) {
  fflush(stream);
  return (long)ftello(stream);
}

/**
 * Resets the file position idicator
 *  @param stream Stream
 */
static inline void rewind(FILE *stream) {
  fseek(stream,0,SEEK_SET);
}

/**
 * Puts a character on stream
 *  @param c Character
 *  @param stream Stream
 *  @return Character
 */
static inline int putc(int c,FILE *stream) {
  return fputc(c,stream);
}

/**
 * Puts a character on stdout
 *  @param c Character
 *  @param stream Stream
 *  @return Character
 */
static inline int putchar(int c) {
  return fputc(c,stdout);
}

/**
 * Gets char from stream
 *  @param stream Stream
 *  @return Character
 */
static inline int fgetc(FILE *stream) {
  char c = EOF;
  _fread(&c,1,stream);
  return c;
}

/**
 * Gets string from stdin
 *  @param s Buffer for string
 *  @return
 */
static inline char* gets(char *s) {
  return fgets(s,BUFSIZ,stdin);
}

/**
 * Gets a character from stream
 *  @param stream Stream
 *  @return Character
 */
static inline int getc(FILE *stream) {
  return fgetc(stream);
}

/**
 * Gets a character from stdin
 *  @return Character
 */
static inline int getchar(void) {
  return fgetc(stdin);
}

/**
 * Reads binary
 *  @param ptr Buffer to store data
 *  @param size Size of each element
 *  @param nitems How many items to read
 *  @param stream Stream to read from
 *  @return How many elements read
 */
static inline size_t fread(void *ptr,size_t size,size_t nitems,FILE *stream) {
  return _fread(ptr,size*nitems,stream);
}

/**
 * Writes error message to stderr
 *  @param str Additional error text (can be NULL)
 */
static inline void perror(const char *str) {
  fprintf(stderr,"%s; %s\n",strerror(errno),str!=NULL?str:"");
}


#endif
