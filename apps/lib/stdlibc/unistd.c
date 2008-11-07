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
