/*
 * Copyright (c) 2008 Mathias Gottschlag and Janosch Graef
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it 
 * and/or modify it under the terms of the Do What The Fuck You Want 
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/projects/COPYING.WTFPL for more details.
 */

#include "draw.h"
#include "cirrus.h"
#include "bitmap.h"
#include "vram.h"

// FIXME: Beschleunigung geht nur für depth >= 8!

static const unsigned char cirrus_rop[] = {
    0x00, // clear
    0x05, // and
    0x09, // andreverse
    0x0D, // copy        Einfaches Setzen der Farbe
    0x50, // andinverted
    0x06, // noop
    0x59, // xor
    0x6D, // or
    0x95, // equiv
    0x0B, // invert
    0xAD, // orreverse
    0xD0, // copyinverted
    0xD6, // orinverted
    0xDA, // nand
    0x0E, // set
};

void cirrus_setup_rop(struct cdi_video_display *display)
{
    volatile struct cirrus_registers *regs = ((struct cirrus_device*)display->device)->regs;
    // ROP nach GR32 schreiben
    cdi_video_raster_op_t rop = ((struct cirrus_display*)display)->rop;
    regs->graph_index = 0x32;
    if (rop == CDI_VIDEO_ROP_COPY) {
        regs->graph_data = cirrus_rop[3];
    } else if (rop == CDI_VIDEO_ROP_OR) {
        regs->graph_data = cirrus_rop[7];
    } else if (rop == CDI_VIDEO_ROP_AND) {
        regs->graph_data = cirrus_rop[1];
    } else if (rop == CDI_VIDEO_ROP_XOR) {
        regs->graph_data = cirrus_rop[6];
    } else {
        regs->graph_data = cirrus_rop[3];
    }
}

void cirrus_set_raster_op(struct cdi_video_display *display, cdi_video_raster_op_t rop)
{
    ((struct cirrus_display*)display)->rop = rop;
}

void cirrus_set_target(struct cdi_video_display *display, struct cdi_video_bitmap *bitmap)
{
    TODO("");
}

void cirrus_draw_dot(struct cdi_video_display *display, unsigned int x, unsigned int y, int color)
{
    // TODO: ROP
    char *framebuffer = (char*)((struct cirrus_device*)display->device)->framebuf;
    int depth = display->mode->depth;
    int displaywidth = display->mode->width;
    int pitch = depth * displaywidth / 8;
    if (depth == 24) {
        char *position = framebuffer + y * pitch + x * 3;
        position[0] = color & 0xFF;
        *((unsigned short*)&position[1]) = (color & 0xFFFF00) >> 8;
    } else if (depth == 16) {
        TODO("");
    } else if (depth == 15) {
        TODO("");
    } else if (depth == 8) {
        TODO("");
    }
}
void cirrus_draw_line(struct cdi_video_display *display, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, int color)
{
    // TODO: Optimieren
    int i;
    int m;
    if (x1 != x2) m = 1000 * ((int)y1 - (int)y2) / ((int)x1 - (int)x2);
    
    if ((m > 1000) || (m < -1000) || (x1 == x2)) {
        m = 1000 * ((int)x1 - (int)x2) / ((int)y1 - (int)y2);
        int n = (int)x1 * 1000 - m * y1;
        if (y2 > y1) {
            for (i = y1; i < y2; i++) {
                cirrus_draw_dot(display, (m * i + n) / 1000, i, color);
            }
        } else {
            for (i = y2; i < y1; i++) {
                cirrus_draw_dot(display, (m * i + n) / 1000, i, color);
            }
        }
    } else {
        int n = (int)y1 * 1000 - m * x1;
        if (x2 > x1) {
            for (i = x1; i < x2; i++) {
                cirrus_draw_dot(display, i, (m * i + n) / 1000, color);
            }
        } else {
            for (i = x2; i < x1; i++) {
                cirrus_draw_dot(display, i, (m * i + n) / 1000, color);
            }
        }
    }
}
void cirrus_draw_rectangle(struct cdi_video_display *display, unsigned int x, unsigned int y, unsigned int width, unsigned int height, int color)
{
    // Clip rect
    if (x < 0) {
        width += x;
        x = 0;
    }
    if (y < 0) {
        height += y;
        y = 0;
    }
    if (x + width >= display->mode->width) {
        width = display->mode->width - x;
    }
    if (y + height >= display->mode->height) {
        height = display->mode->height - y;
    }
    if ((width <= 0) || (height <= 0)) return;
    
    cirrus_setup_rop(display);
    
    volatile struct cirrus_registers *regs = ((struct cirrus_device*)display->device)->regs;
    int depth = display->mode->depth;
    int displaywidth = display->mode->width;
    int pitch = depth * displaywidth / 8;
    
    // Initialize
    regs->graph_index = 0x33;
    regs->graph_data = 0x04;
    
    // Set color depth
    regs->graph_index = 0x30;
    regs->graph_data = 0xC0 | ((depth - 8) << 1);
    
    // Set color
    regs->graph_index = 0x01;
    regs->graph_data = color & 0xFF;
    regs->graph_index = 0x11;
    regs->graph_data = (color & 0xFF00) >> 8;
    regs->graph_index = 0x13;
    regs->graph_data = (color & 0xFF0000) >> 16;
    regs->graph_index = 0x15;
    regs->graph_data = 0;
    
    // Set pitch
    regs->graph_index = 0x24;
    regs->graph_data = pitch & 0xFF;
    regs->graph_index = 0x25;
    regs->graph_data = (pitch & 0x1f00) >> 8;
    
    // Width
    regs->graph_index = 0x20;
    regs->graph_data = (width * 3 - 1) & 0xFF;
    regs->graph_index = 0x21;
    regs->graph_data = ((width * 3 - 1) & 0x1F00) >> 8;
    // Height
    regs->graph_index = 0x22;
    regs->graph_data = height - 1;
    regs->graph_index = 0x23;
    regs->graph_data = ((height - 1) & 0x0700) >> 8;
    // Position
    int dest = pitch * y + x * depth / 8;
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
void cirrus_draw_ellipse(struct cdi_video_display *display, unsigned int x, unsigned int y, unsigned int width, unsigned int height, int color)
{
    // TODO: Optimize this
    int cx = width / 2;
    int cy = height / 2;
    int ix, iy;
    int maxdist = cx * cx;
    for (iy = 0; iy < height; iy++) {
        for (ix = 0; ix < width; ix++) {
            int dist = (((iy - cy) * (int)width) / (int)height) * (((iy - cy) * (int)width) / (int)height) + (ix - cx) * (ix - cx);
            if (dist <= maxdist) {
                cirrus_draw_dot(display, ix + x, iy + y, color);
            }
        }
    }
}

void cirrus_copy_screen(struct cdi_video_display *display, unsigned int x, unsigned int y, unsigned int srcx, unsigned int srcy, unsigned int width, unsigned int height)
{
    // Draw bitmap
    cirrus_setup_rop(display);
    
    int depth = display->mode->depth;
    int destwidth = display->mode->width;
    int destpitch = destwidth * depth / 8;
    
    vram_addr_t dest = y * destpitch + x * depth / 8;
    vram_addr_t src = srcy * destpitch + srcx * depth / 8;
    
    cirrus_copy(display, src, destpitch, dest, destpitch, width, height, 1);
}

void cirrus_draw_bitmap(struct cdi_video_display *display, struct cdi_video_bitmap *bitmap, unsigned int x, unsigned int y)
{
    struct cirrus_bitmap *cirrusbitmap = (struct cirrus_bitmap*)bitmap;
    // Bitmap has to be loaded into VRAM
    // TODO: Direct copy
    if (!cirrusbitmap->vramaddr) {
        if (!cirrus_bitmap_upload(display->device, cirrusbitmap)) {
            printf("cirrus: Cannot upload bitmap!\n");
            return;
        }
    }
    
    // Draw bitmap
    cirrus_setup_rop(display);
    
    int destdepth = display->mode->depth;
    int destwidth = display->mode->width;
    int destpitch = destwidth * destdepth / 8;
    int srcwidth = bitmap->width;
    int srcpitch = srcwidth * destdepth / 8;
    
    vram_addr_t dest = y * destpitch + x * destdepth / 8;
    vram_addr_t src = cirrusbitmap->vramaddr;
    
    cirrus_copy(display, src, srcpitch, dest, destpitch, bitmap->width, bitmap->height, 1);
}

void cirrus_draw_bitmap_part(struct cdi_video_display *display, struct cdi_video_bitmap *bitmap, unsigned int x, unsigned int y, unsigned int srcx, unsigned int srcy, unsigned int width, unsigned int height)
{
    struct cirrus_bitmap *cirrusbitmap = (struct cirrus_bitmap*)bitmap;
    // Bitmap has to be loaded into VRAM
    // TODO: Direct copy
    if (!cirrusbitmap->vramaddr) {
        if (!cirrus_bitmap_upload(display->device, cirrusbitmap)) {
            printf("cirrus: Cannot upload bitmap!\n");
            return;
        }
    }
    
    // Draw bitmap
    cirrus_setup_rop(display);
    
    int destdepth = display->mode->depth;
    int destwidth = display->mode->width;
    int destpitch = destwidth * destdepth / 8;
    int srcwidth = bitmap->width;
    int srcpitch = srcwidth * destdepth / 8;
    
    vram_addr_t dest = y * destpitch + x * destdepth / 8;
    vram_addr_t src = cirrusbitmap->vramaddr + srcy * srcpitch + srcx * destdepth / 8;
    
    cirrus_copy(display, src, srcpitch, dest, destpitch, width, height, 1);
}

