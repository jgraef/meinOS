/*
 * Copyright (c) 2008 Mathias Gottschlag and Janosch Graef
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it 
 * and/or modify it under the terms of the Do What The Fuck You Want 
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/projects/COPYING.WTFPL for more details.
 */

#ifndef _CIRRUS_H_
#define _CIRRUS_H_

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

#include "cdi/video.h"
#include "cdi/pci.h"

#define TODO(s) printf("cirrus: TODO: %s: %s line %d: %s\n",__func__,__FILE__,__LINE__,s)

#define CIRRUS_VENDORID 0x1013
#define CIRRUS_DEVICEID 0x00B8

/**
 * VGA registers as they lie in the register memory area of the card
 */
struct cirrus_registers {
    union {
        uint8_t attr_index;
        uint8_t attr_data_w;
    };
    uint8_t attr_data_r;
    union {
        uint8_t in_stat_0;
        uint8_t misc_out_w;
    };
    uint8_t enable;
    uint8_t seq_index;
    uint8_t seq_data;
    uint8_t dac_mask;
    uint8_t dac_read_addr;
    uint8_t dac_write_addr;
    uint8_t dac_data;
    uint8_t feature_r;
    uint8_t reserved1;
    uint8_t misc_out_r;
    uint8_t reserved2;
    uint8_t graph_index;
    uint8_t graph_data;
    uint8_t iobase_color;
};

struct cirrus_bitmap {
    struct cdi_video_bitmap bitmap;
    // Address of bitmap loaded into VRAM, or 0 if not loaded
    int vramaddr;
};

/**
 * Display connected to a Cirrus device
 */
struct cirrus_display {
    // CDI video display
    struct cdi_video_display dis;

    // List of possible modes
    cdi_list_t modes;
    // Current raster op
    cdi_video_raster_op_t rop;
};

struct cirrus_device {
    // CDI video device
    struct cdi_video_device dev;

    // PCI device
    struct cdi_pci_device *pci;

    // One and only display
    struct cirrus_display *display;

    // Framebuffer
    void *framebuf;
    size_t framebuf_size;

    // VGA Registers
    struct cirrus_registers *regs;
    
    // Bitmaps
    cdi_list_t bitmaps;
    
    // Memory Management data
    struct cirrus_malloc_info *mm_first;
};

// Device
struct cirrus_device *cirrus_device_create(const char *name, struct cdi_pci_device *pci);
void cirrus_device_init(struct cdi_device *device);
void cirrus_device_remove(struct cdi_device *device);

// Display
struct cirrus_display *cirrus_display_create(void);
int cirrus_display_enable(struct cdi_video_display *dis);
int cirrus_display_disable(struct cdi_video_display *dis);
int cirrus_display_clear(struct cdi_video_display *dis);
int cirrus_display_set_mode(struct cdi_video_display *dis, cdi_video_mode_t *mode);
cdi_list_t cirrus_display_get_mode_list(struct cdi_video_display *dis);

// Debug
static inline void DEBUG(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    printf("cirrus: ");
    vprintf(fmt, args);
    va_end(args);
}

// Error
static inline void ERROR(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "cirrus: ");
    vfprintf(stderr, fmt, args);
    va_end(args);
}


#endif
