#include <sys/types.h>
#include <pwd.h>
#include <llist.h>
#include <stdio.h>
#include <stdlib.h>

static llist_t pwd_list = NULL;
static size_t pwd_idx = 0;

static void pwd_init() {
  char *filename;
  asprintf(&filename,"%s/etc/passwd",getenv("ETC"));
  FILE *fd = fopen(filename,"r");
  free(filename);
  /// @todo Read from file and do some scanf or regex stuff
  fclose(fd);
}

struct passwd *getpwnam(const char *name) {
  if (pwd_list==NULL) pwd_init();

  size_t i;
  struct passwd *pwd;
  for (i=0;(pwd = llist_get(pwd_list,i));i++) {
    if (strcmp(pwd->pw_name,name)==0) return pwd;
  }
  return NULL;
}

struct passwd *getpwuid(uid_t uid) {
  if (pwd_list==NULL) pwd_init();

  size_t i;
  struct passwd *pwd;
  for (i=0;(pwd = llist_get(pwd_list,i));i++) {
    if (pwd->pw_uid==uid) return pwd;
  }
  return NULL;
}

struct passwd *getpwent() {
  return llist_get(pwd_list,pwd_idx++);
}

void setpwent() {
  pwd_idx = 0;
}
