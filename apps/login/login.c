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

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <pwd.h>

#define DEFAULT_SHELL "sh"

#ifdef LOGIN
FILE *console;

int init_input() {
  console = fopen("/dev/console","rw");
  return console!=NULL?0:-1;
}

char *input(int hide) {
  static char buf[LOGIN_NAME_MAX];
  int i = 0;
  for (i=0;i<LOGIN_NAME_MAX-1 && (buf[i] = fgetc(console))!='\n';i++) fputc(hide?'*':buf[i],console);
  fputc('\n',console);
  buf[i] = 0;
  return buf;
}

char *login() {
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
  return passwd->pw_shell;
}
#endif

void shell_run(char *shell) {
  printf("login: execute(%s)\n",shell);
while (1);
  //pid_t pid = execute(shell);
  //waitpid(pid);
}

int main(int argc,char *argv[]) {
#ifdef LOGIN
  if (init_input()==-1) abort();
  while (1) shell_run(login());
#else
  shell_run(DEFAULT_SHELL);
#endif
  printf("TODO %s:%d: Run shutdown\n",__FILE__,__LINE__);
  while (1);
  return 0;
}
