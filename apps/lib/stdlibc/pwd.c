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
#include <pwd.h>
#include <llist.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

static llist_t pwd_list = NULL;
static size_t pwd_idx = 0;

static void pwd_shutdown() {
  struct passwd *pwd;
  while (pwd = llist_pop(pwd_list)) {
    free(pwd->pw_name);
    free(pwd->pw_dir);
    free(pwd->pw_shell);
    free(pwd);
  }
  llist_destroy(pwd_list);
}
#include <misc.h>
static void pwd_init() {
  atexit(pwd_shutdown);
  pwd_list = llist_create();

  FILE *fd = fopen("/boot/etc/passwd","r");
  if (fd!=NULL) {
    char *username = malloc(LOGIN_NAME_MAX);
    char *home = malloc(PATH_MAX);
    char *shell = malloc(PATH_MAX);
    char *tmp = malloc(1024);
    uid_t uid;
    gid_t gid;

    while (!feof(fd)) {
      if (fscanf(fd,"%s:%s:%u:%u:%s:%s:%s",username,tmp,&uid,&gid,tmp,home,shell)>0) {
        struct passwd *pwd = malloc(sizeof(struct passwd));
        pwd->pw_name = strdup(username);
        pwd->pw_uid = uid;
        pwd->pw_gid = gid;
        pwd->pw_dir = strdup(username);
        pwd->pw_shell = strdup(username);
        dbgmsg("<%s:%d:%d:%s:%s>\n",pwd->pw_name,pwd->pw_uid,pwd->pw_gid,pwd->pw_dir,pwd->pw_shell);
        llist_push(pwd_list,pwd);
      }
      else break;
    }
    fclose(fd);

    free(username);
    free(home);
    free(shell);
    free(tmp);
  }
}

struct passwd *getpwnam(const char *name) {
  if (pwd_list==NULL) pwd_init();
while (1);
  size_t i;
  struct passwd *pwd;
  for (i=0;(pwd = llist_get(pwd_list,i));i++) {
    if (strcmp(pwd->pw_name,name)==0) return pwd;
  }
  return NULL;
}

struct passwd *getpwuid(uid_t uid) {
  dbgmsg("Hello\n");
  if (pwd_list==NULL) pwd_init();
  dbgmsg("World\n");
while (1);
  size_t i;
  struct passwd *pwd;
  for (i=0;(pwd = llist_get(pwd_list,i));i++) {
    if (pwd->pw_uid==uid) return pwd;
  }
  return NULL;
}

struct passwd *getpwent() {
  if (pwd_list==NULL) pwd_init();
while (1);
  return llist_get(pwd_list,pwd_idx++);
}

void setpwent() {
  pwd_idx = 0;
}
