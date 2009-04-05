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

static int passwd_readline(FILE *fd,char **username,char **home,char **shell,uid_t *uid,gid_t *gid) {
  static char buf[1024];
  fgets(buf,1024,fd);

  // Username
  char *end_username = strchr(buf,':');
  if (end_username==NULL) return -1;
  *end_username = 0;
  *username = buf;

  // Password
  char *end_password = strchr(end_username+1,':');
  if (end_password==NULL) return -1;

  // UID
  char *end_uid = strchr(end_password+1,':');
  if (end_uid==NULL) return -1;
  *end_uid = 0;
  *uid = (uid_t)strtoul(end_password+1,NULL,10);

  // UID
  char *end_gid = strchr(end_uid+1,':');
  if (end_gid==NULL) return -1;
  *end_gid = 0;
  *gid = (uid_t)strtoul(end_uid+1,NULL,10);

  // Comment
  char *end_comment = strchr(end_gid+1,':');
  if (end_comment==NULL) return -1;

  // Home
  char *end_home = strchr(end_comment+1,':');
  if (end_home==NULL) return -1;
  *end_home = 0;
  *home = end_comment+1;

  // Shell
  char *end_shell = end_comment+strlen(end_comment);
  if (end_shell==NULL) return -1;
  *end_shell = 0;
  *shell = end_home+1;

  return 0;
}

static void pwd_init() {
  atexit(pwd_shutdown);
  pwd_list = llist_create();

  FILE *fd = fopen("/boot/etc/passwd","r");
  if (fd!=NULL) {
    char *username;
    char *home;
    char *shell;
    uid_t uid;
    gid_t gid;

    while (!feof(fd)) {
      if (passwd_readline(fd,&username,&home,&shell,&uid,&gid)==0) {
        struct passwd *pwd = malloc(sizeof(struct passwd));
        pwd->pw_name = strdup(username);
        pwd->pw_uid = uid;
        pwd->pw_gid = gid;
        pwd->pw_dir = strdup(home);
        pwd->pw_shell = strdup(shell);
        llist_push(pwd_list,pwd);
      }
      else break;
    }
    fclose(fd);
  }
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
  if (pwd_list==NULL) pwd_init();

  return llist_get(pwd_list,pwd_idx++);
}

void setpwent() {
  pwd_idx = 0;
}
