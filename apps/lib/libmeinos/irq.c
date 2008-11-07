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
