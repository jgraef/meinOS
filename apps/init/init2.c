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
#include <unistd.h>
#include <fcntl.h>

#define INIT_D "/boot/etc/init.d/initd.conf"

int main() {



  while (1);

  FILE *fd = fopen(INIT_D,"r");
  int flags = fcntl(fileno(fd),F_GETFD);
  fcntl(fileno(fd),flags|FD_CLOEXEC);

  if (fd!=NULL) {
    char buf[1024];
    while (!feof(fd)) {
      fgets(buf,1024,fd);
      if (buf[0]!=0 && buf[0]!='\n' && buf[0]!='#') {
        dbgmsg("system(%s) = %d\n",buf,system(buf));
while (1);
      }

#if 0 /** @todo remove */
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

        if (fork()==0) execv(argv[0],argv);

        for (i=0;argv[i];i++) free(argv[i]);
        free(argv);
#endif
    }
    fclose(fd);
    return 0;
  }
  else return 1;
}
