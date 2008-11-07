/*
 * Copyright (c) 2008 Mathias Gottschlag and Janosch Graef
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it 
 * and/or modify it under the terms of the Do What The Fuck You Want 
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/projects/COPYING.WTFPL for more details.
 */

#include <stddef.h>

#include "cirrus.h"
#include "vram.h"

extern void cirrus_setup_rop(struct cdi_video_display *display);

void cirrus_copy(struct cdi_video_display *display, vram_addr_t src, int srcpitch, vram_addr_t dest, int destpitch, int width, int height, int use_rop)
{
    volatile struct cirrus_registers *regs = ((struct cirrus_device*)display->device)->regs;
    if (use_rop) {
        cirrus_setup_rop(display);
    } else {
        // Use COPY rop
        regs->graph_index = 0x32;
        regs->graph_data = 0x0D;
    }
    
    // Destination pitch
    regs->graph_index = 0x24;
    regs->graph_data = destpitch & 0xFF;
    regs->graph_index = 0x25;
    regs->graph_data = (destpitch & 0x1f00) >> 8;
    // Source pitch
    regs->graph_index = 0x26;
    regs->graph_data = srcpitch & 0xFF;
    regs->graph_index = 0x27;
    regs->graph_data = (srcpitch & 0x1f00) >> 8;
    
    int ww = (width * display->mode->depth / 8) - 1;
    int hh = height - 1;
    
    // From which side do we start?
    int decrement = 0;
    if (dest > src) {
        decrement = 1;
        
        dest += hh * destpitch + ww;
        src += hh * srcpitch + ww;
    }
    regs->graph_index = 0x30;
    regs->graph_data = decrement;
    
    // Width
    regs->graph_index = 0x20;
    regs->graph_data = ww & 0xFF;
    regs->graph_index = 0x21;
    regs->graph_data = (ww & 0x1f00) >> 8;
    // Height
    regs->graph_index = 0x22;
    regs->graph_data = hh & 0xFF;
    regs->graph_index = 0x23;
    regs->graph_data = (hh & 0x0700) >> 8;
    // Source
    regs->graph_index = 0x2C;
    regs->graph_data = src & 0xFF;
    regs->graph_index = 0x2D;
    regs->graph_data = (src & 0xFF00) >> 8;
    regs->graph_index = 0x2E;
    regs->graph_data = (src & 0x3F0000) >> 16;
    // Destination
    regs->graph_index = 0x28;
    regs->graph_data = dest & 0xFF;
    regs->graph_index = 0x29;
    regs->graph_data = (dest & 0xFF00) >> 8;
    regs->graph_index = 0x2A;
    regs->graph_data = (dest & 0x3F0000) >> 16;

    // Start drawing
    regs->graph_index = 0x31;
    regs->graph_data = 0x02;
}

#define PTR2VRAMADDR(device,ptr) ((vram_addr_t)(ptr-((device)->framebuf)))

static inline void *cirrus_memcpy(struct cirrus_device *device, void *dest, void *src, size_t size)
{
    cirrus_copy((struct cdi_video_display*)device->display, PTR2VRAMADDR(device, src), size, PTR2VRAMADDR(device, dest), size, (size*8)/device->display->dis.mode->depth, 1, 0);
    return dest;
}

static struct cirrus_malloc_info *block_find_free(struct cirrus_device *device, size_t size)
{
    struct cirrus_malloc_info *cur = device->mm_first;
    
    while (cur->next!=NULL) {
        if (cur->size==size) {
            // Fits perfect
            cur->prev->next = cur->next;
            cur->next->prev = cur->prev;
            return cur;
        }
        else if (cur->size>size+sizeof(struct cirrus_malloc_info)) {
            // Split it
            struct cirrus_malloc_info *new = ((void*)cur)+size;
            new->size = cur->size-size;
            new->prev = cur->prev;
            new->next = cur->next;
            cur->size = size;
            return cur;
        }
         
        // Next block
        cur = cur->next;
    }
    
    return NULL;
}

static void block_free(struct cirrus_device *device, struct cirrus_malloc_info *block)
{
    struct cirrus_malloc_info *cur = device->mm_first;
    
    if (((void*)block)+block->size<(void*)cur) {
        // Put block before first block
        block->next = cur;
        block->prev = NULL;
        cur->prev = block;
        device->mm_first = block;
    }
    else {
        // Find right place in list
        while (cur!=NULL) {
    	    if (((void*)cur)+cur->size<(void*)block && (((void*)block)+block->size<(void*)cur->next || cur->next==NULL)) {
    	        // Put block here in list
                block->next = cur->next;
                block->prev = cur;
                if (cur->next!=NULL) cur->next->prev = block;
                cur->next = block;
    	    }
    	    /// @todo Join two/three blocks
            cur = cur->next;
        }
    }
    
    ERROR("%s: Cannot free VRAM memory: 0x%x\n", device->dev.dev.name, block);
}

void cirrus_vram_setmem(struct cirrus_device *device, void *start, size_t size)
{
    device->mm_first = start;
    device->mm_first->size = size;
    device->mm_first->prev = NULL;
    device->mm_first->next = NULL;
}

void *cirrus_malloc(struct cirrus_device *device, size_t size)
{
    struct cirrus_malloc_info *block = block_find_free(device, size+sizeof(struct cirrus_malloc_info));
    if (block==NULL) return NULL;
    else return ((void*)block)+sizeof(struct cirrus_malloc_info);
}

void cirrus_free(struct cirrus_device *device, void *mem)
{
    struct cirrus_malloc_info *block = mem-sizeof(struct cirrus_malloc_info);
    block_free(device, block);
}
