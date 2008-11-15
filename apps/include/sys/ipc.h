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

#ifndef _SYS_IPC_H_
#define _SYS_IPC_H_

#include <sys/types.h>

#define IPC_CREAT  0x200
#define IPC_EXCL   0x400
#define IPC_NOWAIT 0x800

#define IPC_PRIVATE 0

#define IPC_RMID 1
#define IPC_SET  2
#define IPC_STAT 3

struct ipc_perm {
  uid_t uid;   // Owner's UID
  gid_t gid;   // Owner's GID
  uid_t cuid;  // Creator's UID
  gid_t cgid;  // Creator's GID
  mode_t mode; // Permissions
};

key_t ftok(const char *file,int x);

#endif
