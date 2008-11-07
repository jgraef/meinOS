#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

ssize_t readv(int fildes,const struct iovec *iov,int iovcnt) {
  size_t i;
  ssize_t ret = 0;
  for (i=0;i<iovcnt;i++) {
    size_t cnt = iov->iov_len;
    do {
      ssize_t tmp = read(fildes,iov->iov_base,iov->iov_len);
      if (tmp==-1) return -1;
      cnt -= tmp;
    }
    while (cnt>0);
    ret += iov->iov_len;
  }
  return ret;
}

ssize_t writev(int fildes,const struct iovec *iov,int iovcnt) {
  size_t i;
  ssize_t ret = 0;
  for (i=0;i<iovcnt;i++) {
    size_t cnt = iov->iov_len;
    do {
      ssize_t tmp = write(fildes,iov->iov_base,iov->iov_len);
      if (tmp==-1) return -1;
      cnt -= tmp;
    }
    while (cnt>0);
    ret += iov->iov_len;
  }
  return ret;
}
