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

#include <stdarg.h>
#include <unistd.h>
#include <proc.h>
#include <sys/stat.h>
#include <rpc.h>
#include <stdio.h>
#include <errno.h>

/// @todo set errno

void _fs_cloexec();

static char *find_file(const char *file) {
  char *path;
  /// @todo fixme!
  asprintf(&path,"/boot/bin/%s",file);
  return path;
}

#include <misc.h>
int execl(const char *path, ... /*, (char *)0 */) {
  va_list args;
  int argc,ret;
  char **argv = NULL;

  va_start(args,path);
  for (argc=0;;argc++) {
    argv = realloc(argv,(argc+1)*sizeof(char*));
    argv[argc] = va_arg(args,char*);
    if (argv[argc]==NULL) break;
    dbgmsg("argv[%d] = %s\n",argc,argv[argc]);
  }

  ret = execve(path,(void*)argv,(void*)environ);

  free(argv);
  va_end(args);

  return ret;
}

int execv(const char *path, char *const argv[]) {
  return execve(path,(void*)argv,(void*)environ);
}

int execle(const char *path, ... /*, (char *)0, char *const envp[]*/) {
  va_list args;
  int argc,ret;
  char **argv = NULL;

  va_start(args,path);
  for (argc=0;;argc++) {
    argv = realloc(argv,(argc+1)*sizeof(char*));
    argv[argc] = va_arg(args,char*);
    if (argv[argc]==NULL) break;
  }

  ret = execve(path,argv,va_arg(args,void*));

  free(argv);
  va_end(args);

  return ret;
}

int execve(const char *path, char *const argv[], char *const envp[]) {
  char *cwd;
  int var;

  // get data that must be inheritted
  mode_t cmask = umask(0777);
  cwd = getcwd(NULL,0);

  // execute
  var = proc_pack_procdata((char**)argv,(char**)envp,cwd,cmask);
  _fs_cloexec();
  errno = rpc_call("proc_exec",RPC_FLAG_SENDTO|RPC_FLAG_NORET,1,path,var);

  // reset data that had to be inheritted
  free(cwd);
  umask(cmask);

  return -1;
}

int execlp(const char *file, ... /*, (char *)0 */) {
  char *path;

  if ((path = find_file(file))!=NULL) {
    va_list args;
    int argc,ret;
    char **argv = NULL;

    va_start(args,file);
    for (argc=0;;argc++) {
      argv = realloc(argv,(argc+1)*sizeof(char*));
      argv[argc] = va_arg(args,char*);
      if (argv[argc]==NULL) break;
    }

    ret = execve(path,argv,environ);

    free(path);
    free(argv);
    va_end(args);

    return ret;
  }
  else return -1;
}

int execvp(const char *file, char *const argv[]) {
  char *path;

  if ((path = find_file(file))!=NULL) {
    int ret = execve(path,argv,environ);
    free(path);
    return ret;
  }
  else return -1;
}
