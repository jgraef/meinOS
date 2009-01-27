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

#include <sys/types.h>
#include <unistd.h>
#include <misc.h>
#include <errno.h>
#include <string.h>

int main() {
  pid_t child;
dbgmsg("test: Hello World\n");
while (1);
  int p[2];
  if (pipe(p)==0) {
    dbgmsg("p[0] = %d\n",p[0]);
    dbgmsg("p[1] = %d\n",p[1]);

    if ((child = fork())==0) {
      dbgmsg("writing to pipe\n");
      //write(p[1],"Hello World\n",16);
      dbgmsg("write\n");
    }
    else {
      char buf[16];

      dbgmsg("reading from pipe\n");
      read(p[0],buf,16);
      buf[15] = 0;
      dbgmsg("read: %s\n",buf);
    }

    dbgmsg("reached end\n");
  }
  else dbgmsg("pipe() failed: (#%d) %s\n",errno,strerror(errno));

  while (1);

}
