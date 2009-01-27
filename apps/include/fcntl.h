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

#ifndef _FCNTL_H_
#define _FCNTL_H_

#include <sys/types.h>

#define R_OK 4 // Test for read permission
#define W_OK 2 // Test for write permission
#define X_OK 1 // Test for execute permission
#define F_OK 0 // Test for existence

#ifndef SEEK_SET
  #define SEEK_SET 0 // Seek from beginning of file
  #define SEEK_CUR 1 // Seek from current position
  #define SEEK_END 2 // Seek from end of file
#endif

#define O_RDONLY   1
#define O_WRONLY   2
#define O_RDWR     3
#define O_CREAT    4
#define O_EXCL     8
#define O_NOCTTY   16
#define O_TRUNC    32
#define O_APPEND   64
#define O_NONBLOCK 128
#define O_SYNC     256
#define O_ACCMODE  3

#define F_RDLCK 1
#define F_UNLCK 2
#define F_WRLCK 3

#define F_DUPFD   1
#define F_GETFD   2
#define F_SETFD   3
#define F_GETFL   4
#define F_SETFL   5
#define F_GETLK   6
#define F_SETLK   7
#define F_SETLKW  8
#define F_GETOWN  9
#define F_SETOWN 10

#define FD_CLOEXEC 1

struct flock {
  short l_type;
  short l_whence;
  off_t l_start;
  off_t l_len;
  pid_t l_pid;
};

int fcntl(int fildes,int cmd,...);
int open(const char *path,int oflag,...);

static inline int creat(const char *path,mode_t mode) {
  return open(path,O_WRONLY|O_CREAT|O_TRUNC,mode);
}

#endif
