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

#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <syscall.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>

#define check_stream(stream) ((stream)!=NULL && (stream)->fh>=0)

static FILE *create_stream(int fh,int oflag) {
  FILE *stream = malloc(sizeof(FILE));
  stream->fh = fh;
  stream->oflag = oflag;
  stream->error = 0;
  stream->eof = 0;
  stream->buf = malloc(BUFSIZ);
  stream->bufcur = stream->buf;
  stream->dobuf = 1;
  return stream;
}

static void stdio_exit() {
  fflush(stdout);
  fflush(stderr);
  fclose(stdin);
  fclose(stdout);
  fclose(stderr);
}

/**
 * Initializes stdio
 */
void stdio_init() {
  stdin = create_stream(STDIN_FILENO,O_RDONLY);
  stdout = create_stream(STDOUT_FILENO,O_WRONLY);
  stderr = create_stream(STDERR_FILENO,O_WRONLY);
  atexit(stdio_exit);
}

/**
 * Converts a oflag string to a oflag number
 *  @param oflag Mode string
 *  @return Mode number
 *  @todo fix
 */
static int oflag_str2num(const char *oflag) {
  int m;
  if (oflag[0]=='r') m = O_RDONLY;
  else if (oflag[0]=='w') m = O_CREAT|O_WRONLY|O_TRUNC;
  else if (oflag[0]=='a') m = O_CREAT|O_APPEND|O_TRUNC;
  else m = -1;
  if (oflag[1]=='+' && m!=-1) m |= O_RDONLY|O_WRONLY;
  return m;
}

/**
 * Creates a name for a temporary file
 *  @param s Buffer
 *  @return Filename
 */
char *tmpnam(char *buf1) {
  static unsigned int tmp_num = 0;
  static char *buf2[L_tmpnam];
  char *buf = buf1!=NULL?buf1:memset(buf2,0,L_tmpnam);
  snprintf(buf,L_tmpnam,"%s/%04x",P_tmpdir,tmp_num++);
  return buf;
}

/**
 * Clears error in file
 *  @param file Filehandle
 */
void clearerr(FILE *file) {
  file->error = 0;
}

/**
 * Gets whether stream reached EOF
 *  @param stream Stream
 *  @return !=0 if EOF
 */
int feof(FILE *stream) {
  if (check_stream(stream)) return stream->eof;
  else {
    errno = EBADF;
    return 0;
  }
}

/**
 * Gets current error of stream
 *  @param steam Stream
 *  @return Error number
 */
int ferror(FILE *stream) {
  if (check_stream(stream)) return stream->error;
  else {
    errno = EBADF;
    return 0;
  }
}

/**
 * Closes file
 *  @param stream Stream
 *  @return 0=success; -1=failure
 */
int fclose(FILE *stream) {
  if (check_stream(stream)) {
    if (fflush(stream)==0) {
      if (close(stream->fh)==0) {
        free(stream->buf);
        free(stream);
      }
    }
  }
  else errno = EBADF;
  return -1;
}

/**
 * Opens a file with Filedescriptor
 *  @param fh Filedescriptor
 *  @param oflag Mode
 *  @return Filehandle
 */
FILE *fdopen(int fh,const char *oflag) {
  return create_stream(fh,oflag_str2num(oflag));
}

/**
 * Maps a stream pointer to a file descriptor
 *  @param stream Stream
 *  @return Filedescriptor
 */
int fileno(FILE *stream) {
  if (check_stream(stream)) return stream->fh;
  else {
    errno = EBADF;
    return 0;
  }
}

/**
 * Opens a file
 *  @param name Path to file
 *  @param oflag Mode
 *  @return Filehandle
 *  @todo truncate
 */
FILE *fopen(const char *name,const char *oflag) {
  int m = oflag_str2num(oflag);
  if (m>0) {
    int fh = open(name,m,0777);
    if (fh>=0) {
      //if ((m&O_TRUNC)==O_TRUNC) ftruncate(fh,0);
      return create_stream(fh,m);
    }
  }
  return NULL;
}

/**
 * Flushs a stream
 *  @param stream Stream to flush
 *  @return Success?
 */
int fflush(FILE *stream) {
  if (stream->dobuf) {
    int ret;
    /*if (stream->fh==STDOUT_FILENO) ret = syscall_call(SYSCALL_PUTSN,3,0,stream->buf,stream->bufcur-stream->buf)==-1?EOF:0;
    else if (stream->fh==STDERR_FILENO) ret = syscall_call(SYSCALL_PUTSN,3,1,stream->buf,stream->bufcur-stream->buf)==-1?EOF:0;
    else {*/
      ret = write(stream->fh,stream->buf,stream->bufcur-stream->buf);
      if (ret>0 && ret<(stream->bufcur-stream->buf)) {
        stream->eof = 1;
        ret = EOF;
      }
    //}
    stream->bufcur = stream->buf;
    return ret;
  }
  return 0;
}

/**
 * Print string to stdout
 *  @param s String
 *  @return How many bytes put
 */
int puts(const char *s) {
  int ret = fputs(s,stdout);
  if (ret>0) {
    putchar('\n');
    ret++;
  }
  return ret;
}

/**
 * Writes binary
 *  @param ptr Buffer to get data from
 *  @param size How many bytes to write
 *  @param stream Stream to write to
 *  @return How many elements written
 */
size_t _fwrite(const void *ptr,size_t size,FILE *stream) {
  if (size==0) return 0;
  if (check_stream(stream)) {
    if (stream->oflag&O_WRONLY) {
      if (stream->dobuf /*&& size<BUFSIZ*/) {
        if (stream->bufcur+size>stream->buf+BUFSIZ) fflush(stream);
        if (size>BUFSIZ) size = BUFSIZ;
        memcpy(stream->bufcur,ptr,size);
        stream->bufcur += size;
        if (memchr(ptr,'\n',size)!=NULL) fflush(stream);
        return size;
      }
      else {
        int ret = write(stream->fh,ptr,size);
        if (ret>0 && ret<(stream->bufcur-stream->buf)) stream->eof = 1;
        return ret;
      }
    }
    else {
      stream->eof = 1;
      return 0;
    }
  }
  else {
    errno = EBADF;
    return -1;
  }
}

/**
 * Gets string from file
 *  @param s Destination for string
 *  @param n How many bytes to read
 *  @param stream Stream to read from
 *  @return String
 */
char *fgets(char *s,int n,FILE *stream) {
  int i,chr;
  if (check_stream(stream)) {
    for (i=0;i<n-1;i++) {
      chr = fgetc(stream);
      if (chr==EOF) break;
      else s[i] = chr;
      if (errno>0) {
        stream->error = errno;
        return NULL;
      }
      if (s[i]==0 || s[i]=='\n') break;
    }
    s[i] = 0;
    return s;
  }
  else {
    errno = EBADF;
    return NULL;
  }
}

/**
 * Reads binary
 *  @param ptr Buffer to store data
 *  @param size How many bytes to read
 *  @param stream Stream to read from
 *  @return How many elements read
 */
size_t _fread(void *ptr,size_t size,FILE *stream) {
  if (check_stream(stream)) {
    ssize_t ret;
    ret = read(stream->fh,ptr,size);
    if (ret>0) {
      if (ret<size) stream->eof = 1;
      return ret;
    }
    else return -1;
  }
  else {
    errno = EBADF;
    return -1;
  }
}

int fgetpos(FILE *stream,fpos_t *pos) {
  if (check_stream(stream)) {
    *pos = ftello(stream);
    return 0;
  }
  else {
    errno = EBADF;
    return -1;
  }
}

int fsetpos(FILE *stream,fpos_t *pos) {
  if (check_stream(stream)) {
    fseeko(stream,*pos,SEEK_SET);
    return 0;
  }
  else {
    errno = EBADF;
    return -1;
  }
}

FILE *freopen(const char *filename,const char *oflag,FILE *stream) {
  if (check_stream(stream)) {
    fflush(stream);
    close(stream->fh);
    stream->oflag = oflag_str2num(oflag);
    stream->eof = 0;
    stream->error = 0;
    stream->fh = open(filename,stream->oflag);
    return stream;
  }
  else {
    errno = EBADF;
    return NULL;
  }
}

/// @todo Implement me
int fscanf(FILE *stream,const char *format,...) {
  return 0;
}

/// @todo Implement me
int ungetc(int c,FILE *stream) {
  return 0;
}
