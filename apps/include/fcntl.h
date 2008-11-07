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

struct flock {
  short l_type;
  short l_whence;
  off_t l_start;
  off_t l_len;
  pid_t l_pid;
};

int open(const char *path,int oflag,...);

static inline int creat(const char *path,mode_t mode) {
  return open(path,O_WRONLY|O_CREAT|O_TRUNC,mode);
}

#endif
