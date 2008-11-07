#ifndef _IRQ_H_
#define _IRQ_H_

int irq_reghandler(unsigned int irq,void *func,void *user_data,int give_irq);
int irq_sleep(unsigned int irq);

#endif
