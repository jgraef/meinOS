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
