#ifndef _PWD_H_
#define _PWD_H_

#include <sys/types.h>

struct passwd {
  char *pw_name;
  uid_t pw_uid;
  gid_t pw_gid;
  char *pw_dir;
  char *pw_shell;
};

struct passwd *getpwnam(const char *name);
struct passwd *getpwuid(uid_t uid);
struct passwd *getpwent();
void setpwent();

static inline void endpwent() {
  // do nothing
}

#endif
