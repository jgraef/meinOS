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

#ifndef _DIRENT_H_
#define _DIRENT_H_

#ifdef NAME_MAX
  #define MAXNAMELEN
#endif

#define rewinddir(d) seekdir(d,0)

struct dirent {
  char *d_name;
};

typedef struct filelist_item DIR;

DIR *opendir(const char *path);
int closedir(DIR *dh);
struct dirent *readdir(DIR *dh);
void seekdir(DIR *dh,long loc);
long telldir(DIR *dirp);

#endif
