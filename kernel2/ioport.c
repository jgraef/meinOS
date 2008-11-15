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

#include <sys/types.h>
#include <ioport.h>
#include <syscall.h>

int ioport_init() {
  iopb_general = malloc(IOPORT_MAXNUM/sizeof(*iopb_general));
  if (syscall_create(SYSCALL_IO_REG,ioport_reg,1)==-1) return -1;
  if (syscall_create(SYSCALL_IO_UNREG,ioport_unreg,1)==-1) return -1;
  return 0;
}

int ioport_reg(unsigned int port) {
  /*if (port>=IOPORT_MAXNUM) return -ENOENT;
  if (proc_current->iopb==NULL) return -EPERM;
  if (IOPORT_ISREG(iopb_general,port)) return -EEXIST;

  IOPORT_REG(iopb_general,port);
  IOPORT_REG(proc_current->iopb,port);*/
  return 0;
}

int ioport_unreg(unsigned int port) {
  /*if (port>=IOPORT_MAXNUM) return -ENOENT;
  if (proc_current->iopb==NULL) return -EPERM;
  if (!IOPORT_ISREG(proc_current->iopb,port)) return -EPERM;

  IOPORT_UNREG(iopb_general,port);
  IOPORT_UNREG(proc_current->iopb,port);*/
  return 0;
}
