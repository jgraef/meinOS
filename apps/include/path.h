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

#ifndef _PATH_H_
#define _PATH_H_

#include <sys/types.h>

typedef struct {
  size_t num_parts;
  char **parts;
  char *data;
  int root;
} path_t;

path_t *path_parse(const char *path);
void path_destroy(path_t *path);
void path_reject_dots(path_t *path);
char *path_output(path_t *path,char *path_str);
size_t path_compare(path_t *path1,path_t *path2);
path_t *path_cat(path_t *path1,path_t *path2);
void path_parent(path_t *path);

#endif
