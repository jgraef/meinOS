#ifndef _CDI_IO_H_
#define _CDI_IO_H_

#include <ioport.h>

#define cdi_inb(port)      inb(port)
#define cdi_inw(port)      inw(port)
#define cdi_inl(port)      inl(port)
#define cdi_outb(port,val) outb(port,val)
#define cdi_outw(port,val) outw(port,val)
#define cdi_outl(port,val) outl(port,val)

#endif
