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

#include <sys/wait.h>
#include <stdlib.h>
#include <misc.h>
#include <limits.h>
#include <stdio.h>
#include <dirent.h>

#define INIT_D "/boot/etc/init.d/initd.conf"

int main() {
  FILE *fd = fopen(INIT_D,"r");
  if (fd!=NULL) {
    char buf[1024];
    while (!feof(fd)) {
      fgets(buf,1024,fd);
      if (buf[0]!=0 && buf[0]!='\n' && buf[0]!='#') {
        size_t i;
        size_t j = 0;
        size_t k = 0;
        char **argv = NULL;
        for (i=0;buf[i];i++) {
          if (buf[i]==':') {
            argv = realloc(argv,(k+1)*sizeof(char*));
            argv[k] = malloc(i-j+1);
            memcpy(argv[k],buf+j,i-j);
            argv[k][i-j] = 0;
            k++;
            j = i+1;
          }
        }
        argv = realloc(argv,(k+2)*sizeof(char*));
        argv[k] = malloc(i-j+1);
        memcpy(argv[k],buf+j,i-j);
        argv[k][i-j] = 0;
        argv[k+1] = NULL;

        /// @todo Maybe set stdin,stdout,stderr to /dev/console or some logfile
#ifdef INIT_WAIT
        pid_t pid = execute(argv[0],argv,NULL,NULL,NULL);
        int status = 0;
        waitpid(pid,&status,0);
#else
        execute(argv[0],argv,NULL,NULL,NULL);
#endif
        for (i=0;argv[i];i++) free(argv[i]);
        free(argv);
      }
    }
    fclose(fd);
  }
  else return 1;

  /// @todo remove
  while (1);
  return 0;
}
