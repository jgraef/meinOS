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
#include <rpc.h>
#include <signal.h>
#include <regex.h>

#include <misc.h>

#define INIT_INITTAB "/boot/etc/inittab"

static int current_runlevel = 0;

static void init_set_runlevel(int new) {
  current_runlevel = new;
}

static int init_get_runlevel(void) {
  return current_runlevel;
}

static int init_load_inittab(const char *file) {
  FILE *fd = fopen(INIT_INITTAB,"r");

  if (fd!=NULL) {
    regex_t regex;
    regmatch_t regmatch[8];
    memset(regmatch,0,sizeof(regmatch));

    // "^[:space:]*[^:]*:[^:]*:[^:]*:[^:]*[:space:]*$"
    // ":?[^:]:?"
    regcomp(&regex,"[^:]*",REG_NEWLINE);

    while (!feof(fd)) {
      char line[1024];

      // read line
      fgets(line,1024,fd);

      // comment
      if (line[0]=='#') {
        continue;
      }

      dbgmsg("%s\n",line);

      // match it
      if (regexec(&regex,line,8,regmatch,0)==0) {
        int i;

        for (i=0;i<8;i++) {
          if (regmatch[i].rm_so!=-1) {
//dbgmsg("Start: %d\n",regmatch[i].rm_so);
//dbgmsg("End:   %d\n",regmatch[i].rm_eo);

            size_t len = regmatch[i].rm_eo-regmatch[i].rm_so;
            char *buf = malloc(len+1);
            memcpy(buf,line+regmatch[i].rm_so,len);
            buf[len] = 0;
            dbgmsg("Match #%d: \"%s\"\n",i,buf);
            free(buf);
          }
          else {
            dbgmsg("Match #%d: no match\n",i);
          }
        }
      }
      else {
        dbgmsg("Error1\n");
      }
while (1);
    }

    regfree(&regex);
    return 0;
  }
  else {
    dbgmsg("Error2\n");
    return -1;
  }
}

int main() {
  kill(1,SIGUSR1);

  /// @todo Put in /etc/init.d
  dbgmsg("Starting ramdisk...\n");
  pid_t pid = fork();
  if (pid==0) {
    execl("/boot/bin/ramdisk","ramdisk",0);
    dbgmsg("Didn't start ramdisk, something went wrong.\n");
    return 0;
  }
  dbgmsg("Mounting ramdisk...\n");
  do sleep(1);
  while (vfs_mount("ramdisk","/",NULL,0)!=-1);

  init_load_inittab(INIT_INITTAB);

  rpc_func(init_set_runlevel,"i",sizeof(int));
  rpc_func(init_get_runlevel,"",0);

  rpc_mainloop(-1);

  return 0;
}
