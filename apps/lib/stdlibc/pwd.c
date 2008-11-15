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
