/*
 * Copyright (c) 2008 Mathias Gottschlag and Janosch Graef
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it 
 * and/or modify it under the terms of the Do What The Fuck You Want 
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/projects/COPYING.WTFPL for more details.
 */

#ifndef _VESA_H_
#define _VESA_H_

#include <stdint.h>
#include <cdi/lists.h>
#include <cdi/video.h>

/**
 * VESA information block, used to get information about the card like video
 * modes
 */
struct vesa_info_block {
    char signature[4];
    char version[2];
    uint32_t oemname;
    uint32_t capabilities;
    uint32_t modeptr;
    uint16_t videomem;
    // VBE 2.0
    uint16_t oemversion;
    uint32_t vendornameptr;
    uint32_t productnameptr;
    uint32_t revisionptr;
    uint16_t modes[111];
    uint8_t  oem[256];
} __attribute__ ((packed));

struct vesa_mode_info {
    uint16_t modeattr;
    uint8_t  windowattra;
    uint8_t  windowattrb;
    uint16_t windowgran;
    uint16_t windowsize;
    uint16_t startsega;
    uint16_t startsegb;
    uint32_t posfunc;
    uint16_t scanline;
    
    uint16_t width;
    uint16_t height;
    uint8_t  charwidth;
    uint8_t  charheight;
    uint8_t  planecount;
    uint8_t  depth;
    uint8_t  banks;
    uint8_t  type;
    uint8_t  banksize;
    uint8_t  imagepages;
    uint8_t  reserved;
    // VBE v1.2+
    uint8_t  redmasksize;
    uint8_t  redfieldpos;
    uint8_t  greenmasksize;
    uint8_t  greenfieldsize;
    uint8_t  bluemasksize;
    uint8_t  bluefieldsize;
    uint8_t  resmasksize;
    uint8_t  resfieldsize;
    uint8_t  dircolormode;
    // VBE v2.0
    uint32_t linearfb;
    uint32_t offscreenmem;
    uint32_t offscreensize;
    
    char reserved2[206];
} __attribute__ ((packed));

struct vesa_mode {
    cdi_video_mode_t mode;
    uint16_t vesamode;
};

struct vesa_info_block vesainfo;

int vesa_initialize(void);

cdi_list_t vesa_get_modes(void);

int vesa_change_mode(cdi_video_mode_t *newmode);

#endif
