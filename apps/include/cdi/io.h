/*
 * Copyright (c) 2009 Janosch Gräf
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/projects/COPYING.WTFPL for more details.
 */

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
