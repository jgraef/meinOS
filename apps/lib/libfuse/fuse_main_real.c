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
#include <fuse.h>

int fuse_main_real(int argc,char *argv[],const struct fuse_operations *op,size_t op_size,void *user_data) {
  struct fuse *fuse = fuse_setup(argc,argv,op,op_size,NULL,0,user_data);
  fuse_loop(fuse);
  fuse_teardown(fuse,fuse->mountpoint);
  return 0;
}
