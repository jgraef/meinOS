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

#include <sys/utsname.h>
#include <limits.h>
#include <stdio.h>

/// @todo WARNING! Fixed values, change them by hand!
static char *sysname = "meinOS";
static char *release = "0.1";
static char *version = "0.1";
/// @todo Maybe measure that (CPUID?)
static char *machine = "i686";
static char *hostfil = "/etc/hostname";

static char *get_nodename() {
  static char hostname[HOST_NAME_MAX];
  FILE *fd = fopen(hostfil,"r");
  if (fd!=NULL) {
    fgets(hostname,HOST_NAME_MAX,fd);
    fclose(fd);
    return hostname;
  }
  else return NULL;
}

int uname(struct utsname *utsbuf) {
  char *nodename = get_nodename();
  if (nodename==NULL) return -1;

  utsbuf->nodename = nodename;
  utsbuf->sysname = sysname;
  utsbuf->release = release;
  utsbuf->version = version;
  utsbuf->machine = machine;

  return 0;
}
