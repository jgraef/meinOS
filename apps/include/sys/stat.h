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

#ifndef _SYS_STAT_H_
#define _SYS_STAT_H_

#include <sys/types.h>

#define S_IFMT   00170000
#define S_IFSOCK 0140000
#define S_IFLNK  0120000
#define S_IFREG  0100000
#define S_IFBLK  0060000
#define S_IFDIR  0040000
#define S_IFCHR  0020000
#define S_IFIFO  0010000
#define S_ISUID  0004000
#define S_ISGID  0002000
#define S_ISVTX  0001000

#define S_ISLNK(m)  (((m)&S_IFMT)==S_IFLNK)
#define S_ISREG(m)  (((m)&S_IFMT)==S_IFREG)
#define S_ISDIR(m)  (((m)&S_IFMT)==S_IFDIR)
#define S_ISCHR(m)  (((m)&S_IFMT)==S_IFCHR)
#define S_ISBLK(m)  (((m)&S_IFMT)==S_IFBLK)
#define S_ISFIFO(m) (((m)&S_IFMT)==S_IFIFO)
#define S_ISSOCK(m) (((m)&S_IFMT)==S_IFSOCK)

#define S_IRWXU 00700
#define S_IRUSR 00400
#define S_IWUSR 00200
#define S_IXUSR 00100

#define S_IRWXG 00070
#define S_IRGRP 00040
#define S_IWGRP 00020
#define S_IXGRP 00010

#define S_IRWXO 00007
#define S_IROTH 00004
#define S_IWOTH 00002
#define S_IXOTH 00001

struct stat {
  dev_t     st_dev;
  ino_t     st_ino;
  mode_t    st_mode;
  nlink_t   st_nlink;
  uid_t     st_uid;
  gid_t     st_gid;
  dev_t     st_rdev;
  off_t     st_size;
  time_t    st_atime;
  time_t    st_mtime;
  time_t    st_ctime;
  blksize_t st_blksize;
  blkcnt_t  st_blocks;
};

struct stat64 {
  dev_t      st_dev;
  ino64_t    st_ino;
  mode_t     st_mode;
  nlink_t    st_nlink;
  uid_t      st_uid;
  gid_t      st_gid;
  dev_t      st_rdev;
  off64_t    st_size;
  time_t     st_atime;
  time_t     st_mtime;
  time_t     st_ctime;
  blksize_t  st_blksize;
  blkcnt64_t st_blocks;
  mode_t     st_attr;
};

#include <misc.h>
static __inline__ int fstat64(int fildes, struct stat64 *buf) {
  dbgmsg("TODO: fstat64: %s:%d\n",__FILE__,__LINE__);
  return -1;
}
static __inline__ int lstat64(const char *pathname, struct stat64 *buf) {
  dbgmsg("TODO: lstat64: %s:%d\n",__FILE__,__LINE__);
  return -1;
}
static __inline__ int stat64(const char *pathname, struct stat64 *buf) {
  dbgmsg("TODO: stat64: %s:%d\n",__FILE__,__LINE__);
  return -1;
}

mode_t umask(mode_t cmask);
int mknod(const char *path,mode_t mode,dev_t dev);
int chmod(const char *fn,mode_t mode);
int fchmod(int fildes,mode_t mode);
int stat(const char *path,struct stat *buf);
int fstat(int fildes, struct stat *buf);

static inline int mkdir(const char *fn,mode_t mode) {
  return mknod(fn,mode|S_IFDIR,0);
}

static inline int mkfifo(const char *fn,mode_t mode) {
  return mknod(fn,mode|S_IFIFO,0);
}

#endif
