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

#ifndef _LLIST2_H_
#define _LLIST2_H_

#include <sys/types.h>

typedef struct llist2_item_S llist2_item_t;

typedef struct {
  size_t size;
  llist2_item_t *first_item;
} llist2_t;

struct llist2_item_S {
  id_t id;
  llist2_item_t *next;
  void *data;
};

#endif /* _LLIST2_H_ */
