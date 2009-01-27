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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

static size_t sizenv() {
  size_t i;
  for (i=0;environ[i];i++);
  return i;
}

static char *findenv(const char *name,size_t *idx) {
  size_t i;
  for (i=0;environ[i];i++) {
    if (strcmp(environ[i],name)) {
      if (idx!=NULL) *idx = i;
      return environ[i];
    }
  }
  return NULL;
}

char *getenv(const char *name) {
  if (name==NULL) {
    errno = EINVAL;
    return NULL;
  }
  if (name[0]==0 && strchr(name,'=')!=NULL) {
    errno = EINVAL;
    return NULL;
  }

  char *env = findenv(name,NULL);
  if (env!=NULL) {
    char *val = strchr(env,'=');
    if (val!=NULL) return val+1;
    else errno = EINVAL;
  }
  else errno = ENOENT;
  return NULL;
}

/**
 * sets an enviroment value
 *  @param envname Name of value
 *  @param envval Value
 *  @param overwrite Whether to overwrite existing values
 *  @return 0=success; -1=failure
 */
int setenv(const char *envname, const char *envval, int overwrite) {
  size_t i;
  char *env;

  if (envname==NULL) {
    errno = EINVAL;
    return -1;
  }
  if (envname[0]==0 && strchr(envname,'=')!=NULL) {
    errno = EINVAL;
    return -1;
  }

  env = findenv(envname,&i);
  if (env!=NULL && !overwrite) return -1;

  if (env==NULL) {
    i = sizenv();
    realloc(environ,(i+2)*sizeof(char*));
    environ[i+1] = NULL;
  }
  else free(env);

  asprintf(&env,"%s=%s\n",envname,envval);
  environ[i] = env;

  return 0;
}

/**
 * Unsets an enviroment value
 *  @param name Name of value
 *  @return 0=success; -1=failure
 */
int unsetenv(const char *name) {
  size_t i;
  char *env;
  size_t size;

  if (name==NULL) {
    errno = EINVAL;
    return -1;
  }
  if (name[0]==0 && strchr(name,'=')!=NULL) {
    errno = EINVAL;
    return -1;
  }

  env = findenv(name,&i);
  if (env==NULL) {
    errno = ENOENT;
    return -1;
  }

  free(env);
  size = sizenv();
  memmove(environ+i,environ+i+1,size-i);
  //environ = realloc(environ,size-1);

  return 0;
}
