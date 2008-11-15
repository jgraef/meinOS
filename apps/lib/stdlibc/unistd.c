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
#include <string.h>
#include <unistd.h>
#include <syscall.h>
#include <signal.h>
#include <errno.h>
#include <pwd.h>

/**
 * Swaps bytes
 *  @param src Source buffer
 *  @param dest Destination buffer
 *  @param size Size of buffers
 */
void swab(const void *src,void *dest,ssize_t size) {
  size -= size%2;
  if (size>0) {
    const char *csrc = (const char*)src;
    char *cdest = (char*)dest;
    size_t i;
    for (i=0;i<size;i++) cdest[size-i-1] = csrc[i];
  }
}

unsigned sleep(unsigned sec) {
  syscall_call(SYSCALL_TIME_SLEEP,1,sec);
  return sec;
}

int usleep(useconds_t usec) {
  if (usec<1000000) {
    syscall_call(SYSCALL_TIME_USLEEP,1,usec);
    return 0;
  }
  else {
    errno = EINVAL;
    return -1;
  }
}

static pid_t child_entry() {
  return 0;
}

pid_t fork() {
  return syscall_call(SYSCALL_PROC_FORK,1,child_entry);
}

/*static void alarm_handler(int sig) {
  raise(SIGALRM);
}*/

unsigned alarm(unsigned seconds) {
  /// @todo call timer syscall
  return 0;
}

useconds_t ualarm(useconds_t useconds,useconds_t interval) {
  /// @todo call timer syscall
  return 0;
}

char *getlogin() {
  struct passwd *pwd = getpwuid(getpid());
  if (pwd==NULL) return NULL;
  else return pwd->pw_name;
}
