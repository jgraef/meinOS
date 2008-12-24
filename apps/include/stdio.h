/*
    meinOS - A unix-like x86 microkernel operating system
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
  int oflag;
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
FILE *fdopen(int fh,const char *oflag);
int fileno(FILE *stream);
FILE *fopen(const char *name,const char *oflag);
int fflush(FILE *stream);
int fgetpos(FILE *stream,fpos_t *pos);
int fsetpos(FILE *stream,fpos_t *pos);
FILE *freopen(const char *filename,const char *oflag,FILE *stream);

int rename(const char *old, const char *new);

int puts(const char *s);
size_t _fwrite(const void *ptr,size_t size,FILE *stream);

char *fgets(char *s,int n,FILE *stream);
size_t _fread(void *ptr,size_t size,FILE *stream);
int ungetc(int c,FILE *stream);

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


static __inline__ int remove(const char *path) {
  return unlink(path);
}

/**
 * Puts a char on stream
 *  @param c Character
 *  @param stream Stream
 *  @return Char written
 */
static __inline__ int fputc(int c,FILE *stream) {
  char chr = c;
  return _fwrite(&chr,1,stream)==1?c:EOF;
}

/**
 * Puts a string on a stream
 *  @param s String to put on stream
 *  @param stream Stream
 *  @return How many bytes written
 */
static __inline__ int fputs(const char *s,FILE *stream) {
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
static __inline__ size_t fwrite(const void *ptr,size_t size,size_t nelem,FILE *stream) {
  return _fwrite(ptr,size*nelem,stream);
}

/**
 * Seeks in stream
 *  @param stream Stream
 *  @param offset Offset
 *  @param whence Whence
 *  @return Success?
 */
static __inline__ int fseeko(FILE *stream,off_t offset,int whence) {
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
static __inline__ int fseek(FILE *stream,long offset,int whence) {
  return fseeko(stream,(off_t)offset,whence);
}

/**
 * Returns file offset in stream
 *  @param stream Stream
 *  @return Offset
 */
static __inline__ off_t ftello(FILE *stream) {
  return lseek(stream->fh,0,SEEK_CUR);
}

/**
 * Returns file offset in stream
 *  @param stream Stream
 *  @return Offset
 */
static __inline__ long ftell(FILE *stream) {
  fflush(stream);
  return (long)ftello(stream);
}

/**
 * Resets the file position idicator
 *  @param stream Stream
 */
static __inline__ void rewind(FILE *stream) {
  fseek(stream,0,SEEK_SET);
}

/**
 * Puts a character on stream
 *  @param c Character
 *  @param stream Stream
 *  @return Character
 */
static __inline__ int putc(int c,FILE *stream) {
  return fputc(c,stream);
}

/**
 * Puts a character on stdout
 *  @param c Character
 *  @param stream Stream
 *  @return Character
 */
static __inline__ int putchar(int c) {
  return fputc(c,stdout);
}

/**
 * Gets char from stream
 *  @param stream Stream
 *  @return Character
 */
static __inline__ int fgetc(FILE *stream) {
  char c = EOF;
  _fread(&c,1,stream);
  if (c==EOF) stream->eof = 1;
  return c;
}

/**
 * Gets string from stdin
 *  @param s Buffer for string
 *  @return
 */
static __inline__ char* gets(char *s) {
  return fgets(s,BUFSIZ,stdin);
}

/**
 * Gets a character from stream
 *  @param stream Stream
 *  @return Character
 */
static __inline__ int getc(FILE *stream) {
  return fgetc(stream);
}

/**
 * Gets a character from stdin
 *  @return Character
 */
static __inline__ int getchar(void) {
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
static __inline__ size_t fread(void *ptr,size_t size,size_t nitems,FILE *stream) {
  return _fread(ptr,size*nitems,stream);
}

/**
 * Writes error message to stderr
 *  @param str Additional error text (can be NULL)
 */
static __inline__ void perror(const char *str) {
  fprintf(stderr,"%s: %s\n",str!=NULL?str:"",strerror(errno));
}


#endif
