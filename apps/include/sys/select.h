#ifndef _SYS_SELECT_H_
#define _SYS_SELECT_H_

#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <time.h>

#define FDSET_SIZE (__NFDBITS*64)
#define __NFDBITS  (sizeof(__fd_mask)*8)

typedef int __fd_mask;

struct timeval {
  time_t      tv_sec;
  suseconds_t tv_usec;
};

typedef struct {
  __fd_mask fds_bits[(FDSET_SIZE/__NFDBITS)];
} fd_set;

static inline void FD_CLR(int fh,fd_set *fdset) {
  fdset->fds_bits[fh/__NFDBITS] &= ~(1<<(fh%__NFDBITS));
}

static inline int FD_ISSET(int fh,fd_set *fdset) {
  return fdset->fds_bits[fh/__NFDBITS]&(1<<(fh%__NFDBITS));
}

static inline void FD_SET(int fh,fd_set *fdset) {
  fdset->fds_bits[fh/__NFDBITS] |= 1<<(fh%__NFDBITS);
}

static inline void FD_ZERO(fd_set *fdset) {
  memset(fdset->fds_bits,0,sizeof(fdset->fds_bits));
}

int select(int fh_max,fd_set *fds_read,fd_set *fds_write,fd_set *fds_error,struct timeval *timeout);
//int pselect(int fh_max,fd_set *fds_read,fd_set *fds_write,fd_set *fds_error,struct timespec *timeout,sigset_t *sigmask);

#endif
