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

#ifndef _PERM_H_
#define _PERM_H_

#include <sys/types.h>
#include <llist.h>

#define PERM_R 1
#define PERM_W 2
#define PERM_X 3

#define PERM_ROOTUID 1
#define PERM_ROOTGID 1

#define perm_ingroup(groups,gid) llist_find((groups),(void*)(owner_gid))

int perm_check(uid_t owner_uid,gid_t owner_gid,uid_t access_uid,uid_t access_gid,mode_t mode,int op);

#endif
