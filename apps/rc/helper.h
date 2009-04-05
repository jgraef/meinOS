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

#ifndef _HELPER_H_
#define _HELPER_H_

typedef struct {
  char *name;
  char *path;
  int priority;
} rc_script_t;

char *rcd_path(int runlevel);
int get_runlevel(void);
int str_to_runlevel(const char *str);
void parse_rc_script(rc_script_t *script,const char *dir,const char *file);
int compare_rc_script(const void *vscript1,const void *vscript2);
int make_rc_symlink(const char *name,int runlevel,int priority,char prefix);

#endif /* _HELPER_H_ */
