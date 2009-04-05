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

#include <rpc.h>
#include <stdio.h>
#include <unistd.h>

#include "helper.h"

int get_runlevel(void) {
  return rpc_call("init_get_runlevel",0);
}

int str_to_runlevel(const char *str) {
  if (str[0]==0) return -1;
  if (str[1]!=0) return -1;
  char chr = str[0];

  if (chr>='0' && chr<'7') return chr-'0';
  else return -1;
}

char *rcd_path(int runlevel) {
  static char *path = "/etc/init.d/rcX.d\n";
  path[14] = '0'+runlevel;
  return path;
}

void parse_rc_script(rc_script_t *script,const char *dir,const char *file) {
  char *name;
  asprintf(&(script->path),"%s/%s",dir,file);
  script->priority = strtoul(file+1,&name,10);
  script->name = strdup(name);
}

int compare_rc_script(const void *vscript1,const void *vscript2) {
  const rc_script_t *script1 = vscript1;
  const rc_script_t *script2 = vscript2;

  if (script1->name==NULL) return 1;
  else if (script2->name==NULL) return -1;
  else return script1->priority-script2->priority;
}

int make_rc_symlink(const char *name,int runlevel,int priority,char prefix) {
  char *link;
  char *target;

  asprintf(&link,"%s/%c%02d%s",rcd_path(runlevel),prefix,priority,name);
  asprintf(&target,"/etc/init.d/%s",name);

  int ret = symlink(target,link);

  free(link);
  free(target);

  return ret;
}
