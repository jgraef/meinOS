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
#include <stdlib.h>
#include <stdio.h>

/*static char *sysname = NULL;
static char *release = NULL;
static char *version = NULL;
static char *machine = NULL;
static char *nodename = NULL;

static void deinit() {
  free(sysname);
  free(release);
  free(version);
  free(machine);
  free(nodename);
}

static char *get_value(char *name) {
  char *path;
  char buf[1024];

  /// TODO: this should be /etc/utsname when symlinks work
  asprintf(&path,"/boot/etc/utsname/%s",name);
  FILE *fd = fopen(path,"r");
  if (fd!=NULL) {
    fgets(buf,1024,fd);
    fclose(fd);
    return strdup(buf);
  }
  else return NULL;
}*/

int uname(struct utsname *utsbuf) {
  /*static int inited = 0;

  if (!inited) {
    /// TODO: loading from CD is to slow at the moment
    sysname = get_value("sysname");
    release = get_value("release");
    version = get_value("version");
    machine = get_value("machine");
    /// @todo this should be a symlink to /etc/hostname
    nodename = get_value("nodename");
    atexit(deinit);
    inited = 1;
  }

  utsbuf->sysname = sysname;
  utsbuf->release = release;
  utsbuf->version = version;
  utsbuf->machine = machine;
  utsbuf->nodename = nodename;*/

  utsbuf->sysname = "meinOS";
  utsbuf->release = "0.1";
  utsbuf->version = "0.1";
  utsbuf->machine = "i686";
  utsbuf->nodename = "asterix";

  return 0;
}
