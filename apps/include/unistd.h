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

#ifndef _UNISTD_H_
#define _UNISTD_H_

#include <sys/types.h>
#include <syscall.h>
#include <stdlib.h>
#include <string.h>
#include <proc.h>
#include <crypt.h>

#ifndef NULL
  #define NULL ((void*)0)
#endif

#define _POSIX_VERSION    200112L
#define _POSIX2_VERSION   200112L

#define SEEK_SET         0 // Seek from beginning of file
#define SEEK_CUR         1 // Seek from current position
#define SEEK_END         2 // Seek from end of file

#define STDIN_FILENO     0
#define STDOUT_FILENO    1
#define STDERR_FILENO    2

#define _exit(r)    _Exit(r)

#define dup(fildes) dup2(fildes,0)

/// @todo When symlinks are (completely) implemented, this has to be rewritten
#define lstat(path,buf) stat(path,buf)

char *optarg;
int optind,opterr,optopt;
char **environ;

int chown(const char *file,uid_t uid,gid_t gid);
int fchown(int fildes,uid_t uid,gid_t gid);
int symlink(const char *path1,const char *path2);
ssize_t readlink(const char *path,char *buf,size_t bufsize);
int symlink(const char *src,const char *dest);
int link(const char *src, const char *dest);
int access(const char *path,int amode);
int chdir(const char *dir);
int fchdir(int fildes);
char *getcwd(char *, size_t);
int dup2(int fildes,int fildes2);
int close(int fildes);
ssize_t read(int fildes,void *buf,size_t nbyte);
ssize_t write(int fildes,const void *buf,size_t count);
ssize_t pread(int fildes,void *buf,size_t count,off_t offset);
ssize_t pwrite(int fildes,const void *buf,size_t count,off_t offset);
off_t lseek(int fildes,off_t offset,int whence);
int unlink(const char *path);
int rmdir(const char *path);
int ftruncate(int fildes,off_t length);
int truncate(const char *path,off_t length);
int pipe(int fildes[2]);
char *ttyname(int fildes);
int isatty(int fildes);

int gethostname(char *, size_t);
void swab(const void *buf1,void *buf2,ssize_t size);
unsigned sleep(unsigned sec);
int usleep(useconds_t usec);

int getopt(int argc, char *const *argv, const char *optstring);

int execl(const char *path, ... /*, (char *)0 */);
int execv(const char *path, char *const argv[]);
int execle(const char *path, ... /*, (char *)0, char *const envp[]*/);
int execve(const char *path, char *const argv[], char *const envp[]);
int execlp(const char *file, ... /*, (char *)0 */);
int execvp(const char *file, char *const argv[]);

pid_t fork();

// ID enum
//  1 - Real ID
//  2 - Effective ID
//  3 - Saved ID
static __inline__ getuid(void) {
  return syscall_call(SYSCALL_PROC_GETUID,1,1);
}
static __inline__ geteuid(void) {
  return syscall_call(SYSCALL_PROC_GETUID,2,1);
}
static __inline__ getgid(void) {
  return syscall_call(SYSCALL_PROC_GETGID,1,1);
}
static __inline__ getegid(void) {
  return syscall_call(SYSCALL_PROC_GETGID,2,1);
}
static __inline__ setuid(uid_t uid) {
  syscall_call(SYSCALL_PROC_SETUID,2,1,uid);
  syscall_call(SYSCALL_PROC_SETUID,2,2,uid);
  syscall_call(SYSCALL_PROC_SETUID,2,3,uid);
}
static __inline__ seteuid(uid_t uid) {
  syscall_call(SYSCALL_PROC_SETUID,2,2,uid);
}
static __inline__ setreuid(uid_t ruid,uid_t euid) {
  syscall_call(SYSCALL_PROC_SETUID,2,1,ruid);
  syscall_call(SYSCALL_PROC_SETUID,2,2,euid);
}
static __inline__ setgid(gid_t gid) {
  syscall_call(SYSCALL_PROC_SETGID,2,1,gid);
  syscall_call(SYSCALL_PROC_SETGID,2,2,gid);
  syscall_call(SYSCALL_PROC_SETGID,2,3,gid);
}
static __inline__ setegid(gid_t gid) {
  syscall_call(SYSCALL_PROC_SETGID,2,2,gid);
}
static __inline__ setregid(gid_t rgid,gid_t egid) {
  syscall_call(SYSCALL_PROC_SETGID,2,1,rgid);
  syscall_call(SYSCALL_PROC_SETGID,2,2,egid);
}

static __inline__ pid_t getpid(void) {
  return syscall_call(SYSCALL_PROC_GETPID,0);
}
static __inline__ pid_t getppid(void) {
  getppidbypid(getpid());
}

#endif
