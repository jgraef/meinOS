#ifndef _UNISTD_H_
#define _UNISTD_H_

#include <sys/types.h>
#include <syscall.h>
#include <stdlib.h>
#include <string.h>
#include <misc.h>

#ifndef NULL
  #define NULL ((void*)0)
#endif

#define POSIX_VERSION    200112L
#define POSIX2_VERSION   200112L

#define SEEK_SET         0 // Seek from beginning of file
#define SEEK_CUR         1 // Seek from current position
#define SEEK_END         2 // Seek from end of file

#define STDIN_FILENO     0
#define STDOUT_FILENO    1
#define STDERR_FILENO    2

#define _exit(r)    _Exit(r)
#define getpid()    syscall_call(SYSCALL_PROC_GETPID,0)
#define getppid()   getppidbypid(getpid())

// ID Bitmask
//  1 - Real ID
//  2 - Effective ID
//  4 - Saved ID

#define getuid()    syscall_call(SYSCALL_PROC_GETUID,1,1)
#define geteuid()   syscall_call(SYSCALL_PROC_GETUID,1,2)
#define getgid()    syscall_call(SYSCALL_PROC_GETGID,1,1)
#define getegid()   syscall_call(SYSCALL_PROC_GETGID,1,2)

#define setuid(uid)   syscall_call(SYSCALL_PROC_SETUID,2,7,uid)
#define seteuid(uid)  syscall_call(SYSCALL_PROC_SETUID,2,2,uid)
#define setreuid(uid) syscall_call(SYSCALL_PROC_SETUID,2,3,uid)
#define setgid(gid)   syscall_call(SYSCALL_PROC_SETGID,2,7,gid)
#define setegid(gid)  syscall_call(SYSCALL_PROC_SETGID,2,2,uid)
#define setregid(gid) syscall_call(SYSCALL_PROC_SETGID,2,3,gid)

#define dup(fildes) dup2(fildes,0)
// meinOS has no "ttys"
#define isatty(fh) 0
/// @todo When symlinks are (completely) implemented, this has to be rewritten
#define lstat(path,buf) stat(path,buf)

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
off_t lseek(int fildes,off_t offset,int whence);
int unlink(const char *path);
int rmdir(const char *path);
int ftruncate(int fildes,off_t length);
int truncate(const char *path,off_t length);

int gethostname(char *, size_t); ///< @todo implement uname(...);
void swab(const void *buf1,void *buf2,ssize_t size);
unsigned sleep(unsigned sec);
int usleep(useconds_t usec);

int getopt(int argc, char *const *argv, const char *optstring);
extern char *optarg;
extern int optind,opterr,optopt;

extern char **environ;
int execv(const char *path,const char *argv[]);
int execve(const char *path,const char *argv[],const char *env);

#endif
