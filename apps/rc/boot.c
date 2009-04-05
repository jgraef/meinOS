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
#include <dirent.h>

#include "helper.h"

static const char *grub_module_template =
"#!/bin/sh"
"# Copyright (c) 2009 Janosch Graef"
"#"
"### BEGIN INIT INFO"
"# Provides:       %s"
"# Required-Start:"
"# Required-Stop:"
"# Should-Start:"
"# Should-Stop:"
"# Default-Start:  1 2 3 5"
"# Default-Stop:"
"# Short-Description: Pseudo RC script for grub module '%s'"
"# Description:       Pseudo RC script for grub module '%s'"
"### END INIT INFO"
""
"case \"$1\" in"
"  start)"
"    echo \"This is a GRUB module. It should already run.\""
"    ;;"
"  stop)"
"    echo \"Cannot stop GRUB modules.\" > 2"
"    ;;"
"  status)"
"    echo \"running\""
"    ;;"
"  *)"
"    echo \"Usage: $0 {start|stop|status}\" > 2"
"    ;;"
"esac";

static int make_grub_module_script(const char *name) {
  char *script;
  char *path;
  int ret = -1;

  asprintf(&path,"/etc/init.d/%s",name);
  asprintf(&script,grub_module_template,name,name,name);

  FILE *fd = fopen(path,"w");
  if (fd!=NULL) {
    fputs(script,fd);
    fclose(fd);
    ret = 0;
  }

  free(path);
  free(script);

  make_rc_symlink(name,1,1,'S');
  make_rc_symlink(name,2,1,'S');
  make_rc_symlink(name,3,1,'S');
  make_rc_symlink(name,5,1,'S');
  make_rc_symlink(name,1,1,'K');
  make_rc_symlink(name,2,1,'K');
  make_rc_symlink(name,3,1,'K');
  make_rc_symlink(name,5,1,'K');

  return ret;
}

int main(int argc,char *argv[]) {
  /// @todo get GRUB modules and create module scripts
  make_grub_module_script("vfs");
  return 0;
}
