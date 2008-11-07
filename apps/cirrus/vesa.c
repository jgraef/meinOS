/*
 * Copyright (c) 2008 Mathias Gottschlag and Janosch Graef
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it 
 * and/or modify it under the terms of the Do What The Fuck You Want 
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/projects/COPYING.WTFPL for more details.
 */

#include "vesa.h"
#include "cdi/bios.h"
#include "cdi/video.h"

#include <stdlib.h>
#include <stdio.h>

struct vesa_info_block vesainfo;

static cdi_list_t modes = 0;

/**
 * Initializes VESA
 * Reads in graphics card info like video modes etc
 * @return 0, if VESA is available, -1 if it is not (because of some error)
 */
int vesa_initialize(void)
{
    // Setup registers/memory
    struct cdi_bios_registers regs;
    memset(&regs, 0, sizeof(regs));
    regs.ax = 0x4F00;
    regs.ds = 0x8000;
    regs.es = 0x8000;
    regs.di = 0x0000;
    memcpy(&vesainfo, "VBE2", 4);
    cdi_list_t memorylist = cdi_list_create();
    struct cdi_bios_memory infomemory;
    infomemory.src = &vesainfo;
    infomemory.dest = 0x80000;
    printf("Size: %d\n", sizeof(struct vesa_info_block));
    infomemory.size = sizeof(struct vesa_info_block);
    memorylist = cdi_list_push(memorylist, &infomemory);
    // Call BIOS
    int result = cdi_bios_int10(&regs, memorylist);
    cdi_list_destroy(memorylist);
    // cdi_bios_int10 not supported?
    if (result == -1) {
        fprintf(stderr, "cirrus: cdi_bios_int10 failed.\n");
        return -1;
    }
    
    printf("VESA: %c%c%c%c\n", vesainfo.signature[0], vesainfo.signature[1], vesainfo.signature[2], vesainfo.signature[3]);
    printf("Version: %d.%d\n", vesainfo.version[1], vesainfo.version[0]);
    printf("Modes: %05X\n", vesainfo.modeptr);
    
    // Read in modes
    modes = cdi_list_create();
    
    int i;
    uint16_t *modeptr = (uint16_t*)((char*)&vesainfo + (vesainfo.modeptr & 0xFFFF));
    for (i = 0; i < 111; i++) {
        if (modeptr[i] == 0xFFFF) break;
        printf("Mode: %04X\n", modeptr[i]);
        
        // Get mode info
        memset(&regs, 0, sizeof(regs));
        regs.ax = 0x4F01;
        regs.cx = modeptr[i];
        regs.ds = 0x8000;
        regs.es = 0x8000;
        regs.di = 0x0000;
        struct vesa_mode_info modeinfo;
        infomemory.src = &modeinfo;
        infomemory.size = 256;
        memorylist = cdi_list_create();
        memorylist = cdi_list_push(memorylist, &infomemory);
        // Call BIOS
        cdi_bios_int10(&regs, memorylist);
        cdi_list_destroy(memorylist);
        printf("Size: %dx%dx%d\n", modeinfo.width, modeinfo.height, modeinfo.depth);
        printf("Framebuffer: %X\n", modeinfo.linearfb);
        printf("Attr: %04X\n", modeinfo.modeattr);
        
        // Push mode onto list
        struct vesa_mode *mode = malloc(sizeof(struct vesa_mode));
        mode->mode.width = modeinfo.width;
        mode->mode.height = modeinfo.height;
        mode->mode.depth = modeinfo.depth;
        mode->mode.type = CDI_VIDEO_GRAPHIC;
        mode->vesamode = modeptr[i];
        modes = cdi_list_push(modes, mode);
    }
    cdi_video_mode_t *mode = malloc(sizeof(cdi_video_mode_t));
    mode->width = 80;
    mode->height = 25;
    mode->depth = 4;
    mode->type = CDI_VIDEO_TEXT;
    modes = cdi_list_push(modes, mode);
    return 0;
}

cdi_list_t vesa_get_modes(void)
{
    return modes;
}

int vesa_change_mode(cdi_video_mode_t *newmode)
{
    // Text mode
    if ((newmode->width == 80)
     && (newmode->height == 25)
     && (newmode->depth == 4)
     && (newmode->type == CDI_VIDEO_TEXT)) {
        struct cdi_bios_registers regs;
        memset(&regs, 0, sizeof(regs));
        regs.ax = 0x0083;
        cdi_bios_int10(&regs, 0);
    }
    // Search for mode number
    uint16_t modenumber = 0xFFFF;
    int i;
    for (i = 0; i < cdi_list_size(modes); i++) {
        struct vesa_mode *mode = cdi_list_get(modes, i);
        if ((cdi_video_mode_t*)mode == newmode) {
            modenumber = mode->vesamode;
            break;
        }
        if ((mode->mode.width == newmode->width)
         && (mode->mode.height == newmode->height)
         && (mode->mode.depth == newmode->depth)
         && (mode->mode.type == newmode->type)) {
            modenumber = mode->vesamode;
            break;
        }
    }
    if (modenumber == 0xFFFF) {
        return -1;
    }
    
    // Change mode
    struct cdi_bios_registers regs;
    memset(&regs, 0, sizeof(regs));
    regs.ax = 0x4F02;
    regs.bx = modenumber | 0xC000;
    cdi_bios_int10(&regs, 0);
    if ((regs.ax & 0xFF00) != 0x4F00) return -1;
    if (regs.ax & 0xFF) return -1;
    
    return 0;
}

