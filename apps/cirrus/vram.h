/*
 * Copyright (c) 2008 Mathias Gottschlag and Janosch Graef
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it 
 * and/or modify it under the terms of the Do What The Fuck You Want 
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/projects/COPYING.WTFPL for more details.
 */

#ifndef _VRAM_H_
#define _VRAM_H_

struct cirrus_malloc_info;
struct cirrus_malloc_info {
    size_t size;
    struct cirrus_malloc_info *prev;
    struct cirrus_malloc_info *next;
};

typedef unsigned int vram_addr_t;

void cirrus_copy(struct cdi_video_display *display, vram_addr_t src, int srcpitch, vram_addr_t dest, int destpitch, int width, int height, int use_rop);
void cirrus_vram_setmem(struct cirrus_device *device, void *start, size_t size);

#endif
