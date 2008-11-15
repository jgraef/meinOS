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

#include <stdlib.h>
#include <string.h>
#include <llist.h>
#include <errno.h>

struct envvar {
  char *name;
  char *val;
};

static llist_t envvars;

void env_init() {
  envvars = llist_create();
  /// @todo load ENV variables
}

static int _findenv(const char *name) {
  if (name!=NULL && strlen(name)!=0 && strchr(name,'=')==NULL) {
    int i;
    struct envvar *env;
    for (i=0;(env = llist_get(envvars,i));i++) {
      if (strcmp(env->name,name)==0) return i;
    }
  }
  errno = EINVAL;
  return -1;
}
#define findenv(name) llist_get(envvars,_findenv(name))

char *getenv(const char *name) {
  struct envvar *env = findenv(name);
  if (env==NULL) return NULL;
  else return env->val;
}

/**
 * sets an enviroment value
 *  @param envname Name of value
 *  @param envval Value
 *  @param overwrite Whether to overwrite existing values
 *  @return 0=success; -1=failure
 */
int setenv(const char *envname, const char *envval, int overwrite) {
  struct envvar *env = findenv(envname);
  if (env==NULL) {
    env = malloc(sizeof(struct envvar));
    env->name = strdup(envname);
    env->val = strdup(envval);
    llist_push(envvars,env);
  }
  else if (overwrite) {
    free(env->val);
    env->val = strdup(envval);
  }
  else return -1;
  return 0;
}

/**
 * Unsets an enviroment value
 *  @param name Name of value
 *  @return 0=success; -1=failure
 */
int unsetenv(const char *name) {
  int i = _findenv(name);
  if (i!=-1) {
    struct envvar *env = llist_remove(envvars,i);
    free(env->name);
    free(env->val);
    free(env);
    return 0;
  }
  else {
    errno = EINVAL;
    return -1;
  }
}
