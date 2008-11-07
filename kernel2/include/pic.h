#ifndef _PIC_H_
#define _PIC_H_

#define PIC1                        0x20     // IO base address for master PIC
#define PIC2                        0xA0     // IO base address for slave PIC
#define PIC1_COMMAND                PIC1
#define PIC1_DATA                   (PIC1+1)
#define PIC2_COMMAND                PIC2
#define PIC2_DATA                   (PIC2+1)
#define PIC_EOI                     0x20     // End - of - interrupt command code
#define ICW1_ICW4                   0x01     // ICW4 (not) needed
#define ICW1_SINGLE                 0x02     // Single (cascade) mode
#define ICW1_INTERVAL4              0x04     // Call address interval 4 (8)
#define ICW1_LEVEL                  0x08     // Level triggered (edge) mode
#define ICW1_INIT                   0x10     // Initialization - required!
#define ICW4_8086                   0x01     // 8086/88 (MCS-80/85) mode
#define ICW4_AUTO                   0x02     // Auto (normal) EOI
#define ICW4_BUF_SLAVE              0x08     // Buffered mode/slave
#define ICW4_BUF_MASTER             0x0C     // Buffered mode/master
#define ICW4_SFNM                   0x10     // Special fully nested (not)

#define PIT_CHANNELS                0x40     // Channel ports of PIT
#define PIT_FREQ                    1193180  // Frequency of PIT

int pic_init();
unsigned int pic_getmask(int irq);
void pic_setmask(int irq,unsigned int mask);
void pic_eoi(int irq);
int pic_pit_setinterval(int channel,unsigned int interval);

#endif
