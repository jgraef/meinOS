/*
    meinOS - A unix-like x86 microkernel operating system
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>

int main() {
  FILE *terminal = fopen("/dev/console","r+");
  fprintf(terminal,"login: This login shell isn't used anymore.\n");
  return 0;
}

#if 0
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <misc.h>
#include <pwd.h>

/// @todo this should be "/bin/sh" (when symlinks work)
#define DEFAULT_SHELL "/boot/bin/sh"
#define DEFAULT_WDIR  "/"

FILE *console;

int init_input(const char *terminal) {
  console = fopen(terminal,"rw");
  return console!=NULL?0:-1;
}

#ifdef LOGIN
char *input(int hide) {
  static char buf[LOGIN_NAME_MAX];
  int i = 0;
  for (i=0;i<LOGIN_NAME_MAX-1 && (buf[i] = fgetc(console))!='\n';i++) fputc(hide?'*':buf[i],console);
  fputc('\n',console);
  buf[i] = 0;
  return buf;
}

char *login(char *wdir) {
  char *user;
  //char *pass;
  struct passwd *passwd;

  do {
    fputs("User: ",console);
    fflush(stdout);
    user = input(0);
    passwd = getpwnam(user);
    if (passwd==NULL) {
      fputs("User does not exist.\n",console);
      break;
    }
    /// @todo Check password
    /*fputs("Password: ",console);
    fflush(stdout);
    pass = input(1);*/
  } while (0);

  setreuid(passwd->pw_uid);
  setregid(passwd->pw_gid);
  chdir(passwd->pw_dir);
  *wdir = passwd->pw_dir;
  return passwd->pw_shell;
}
#endif

void shell_run(char *shell,char *wdir,char *terminal) {
  char *argv[] = {shell,"-w",wdir,"-t",terminal,NULL};
  pid_t pid = execute(shell,argv,NULL,NULL,NULL);
  waitpid(pid,NULL,0);
}

int main(int argc,char *argv[]) {
  char *terminal = argc>1?argv[1]:"/dev/console";

  if (init_input(terminal)==-1) abort();
#ifdef LOGIN
  while (1) {
    char *wdir;
    char *shell = login(&wdir);
    shell_run(shell,wdir,user);
  }
#else
  shell_run(DEFAULT_SHELL,DEFAULT_WDIR,terminal);
#endif
while (1);
  computer_shutdown();
  return 0;
}
#endif
