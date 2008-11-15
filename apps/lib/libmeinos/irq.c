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
#include <irq.h>
#include <llist.h>
#include <syscall.h>

#include <stdio.h>

struct irq_handler {
  void *func;
  void *user_data;
  int give_irq;
};

static llist_t irq_handlers[16];

int _irq_init() {
  size_t i;
  for (i=0;i<16;i++) irq_handlers[i] = llist_create();
  return 0;
}

void _irq_handler_stub(unsigned int irq);

int irq_reghandler(unsigned int irq,void *func,void *user_data,int give_irq) {
  if (irq<16) {
    if (llist_empty(irq_handlers[irq])) {
      if (syscall_call(SYSCALL_IRQ_HANDLER,2,irq,_irq_handler_stub)==-1) return -1;
    }
    struct irq_handler *new = malloc(sizeof(struct irq_handler));
    new->func = func;
    new->user_data = user_data;
    new->give_irq = give_irq;
    llist_push(irq_handlers[irq],new);
    return 0;
  }
  else return -1;
}

void _irq_handler(unsigned int irq) {
  if (irq<16) {
    size_t i;
    struct irq_handler *handler;
    for (i=0;(handler = llist_get(irq_handlers[irq],i));i++) {
      if (handler->give_irq) {
        void (*func)(unsigned int irq,void *user_data);
        func = handler->func;
        func(irq,handler->user_data);
      }
      else {
        void (*func)(void *user_data);
        func = handler->func;
        func(handler->user_data);
      }
    }
  }
}

int irq_sleep(unsigned int irq) {
  if (irq<16) {
    syscall_call(SYSCALL_IRQ_SLEEP,1,irq);
    return 0;
  }
  else return -1;
}
