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

#include <fuse.h>
#include <string.h>

int fuse_parse_cmdline(struct fuse_args *args,char **mountpoint,int *multithreaded,int *foreground) {
  size_t i;
  *mountpoint = NULL;
  *foreground = 0;
  for (i=1;i<args->argc;i++) {
    if (args->argv[i][0]=='-') { // Switch
      if (strcmp(args->argv[i],"-m")==0) *multithreaded = 1;
      if (strcmp(args->argv[i],"-f")==0) *foreground = 1;
    }
    else *mountpoint = args->argv[i];
  }
  return 0;
}
