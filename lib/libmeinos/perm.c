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

#include <sys/types.h>
#include <perm.h>

/**
 * Check permissions
 *
 */
int perm_check(uid_t owner_uid,gid_t owner_gid,uid_t access_uid,uid_t access_gid,mode_t mode,int op) {
  int opmode = 0;
  if (access_uid==PERM_ROOTUID || access_gid==PERM_ROOTGID) return 1;
  if (op==PERM_R) opmode = mode&0444;
  if (op==PERM_W) opmode = mode&0222;
  if (op==PERM_X) opmode = mode&0111;
  if (access_uid==owner_uid && (opmode&0700)) return 1;
  if (access_gid==owner_gid && (opmode&0070)) return 1;
  if ((opmode&0007)) return 1;
  return 0;
}
