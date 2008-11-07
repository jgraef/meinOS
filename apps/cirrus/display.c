/*
 * Copyright (c) 2008 Mathias Gottschlag and Janosch Graef
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it 
 * and/or modify it under the terms of the Do What The Fuck You Want 
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/projects/COPYING.WTFPL for more details.
 */

#include <stdlib.h>

#include "cdi/lists.h"
#include "cdi/video.h"

#include "cirrus.h"
#include "vesa.h"
#include "vram.h"

/**
 * Creates a new display and adds it to the device
 */		
struct cirrus_display *cirrus_display_create()
{
    struct cirrus_display *display = malloc(sizeof(struct cirrus_display));

    display->dis.activated = 0;
    display->dis.mode = NULL;
    display->modes = cdi_list_create();
    display->rop = CDI_VIDEO_ROP_COPY;

    display->modes = vesa_get_modes();

    return display;
}

/**
 * Enables the display
 */
int cirrus_display_enable(struct cdi_video_display *dis)
{
    struct cirrus_display *display = (struct cirrus_display*)dis;
    if (!display->dis.activated) {
        TODO("");
        if (display->dis.mode) {
            vesa_change_mode(display->dis.mode);
        }
        display->dis.activated = 1;
    }
    return -1;
}

/**
 * Disables the display
 */
int cirrus_display_disable(struct cdi_video_display *dis)
{
    struct cirrus_display *display = (struct cirrus_display*)dis;
    if (display->dis.activated) {
        cirrus_display_clear(dis);
        display->dis.activated = 0;
    }
    return -1;
}

/**
 * Clears the given display to black
 */
int cirrus_display_clear(struct cdi_video_display *dis)
{
    struct cirrus_display *display = (struct cirrus_display*)dis;
    if (display->dis.activated) {
        TODO("");
    }
    return -1;
}

/**
 * Sets the mode of the display
 * @param dis Display to be changed
 * @param mode New mode of the display
 */
int cirrus_display_set_mode(struct cdi_video_display *dis, cdi_video_mode_t *mode)
{
    struct cirrus_display *display = (struct cirrus_display*)dis;
    struct cirrus_device *device = (struct cirrus_device*)(dis->device);
    
    if (display->dis.activated) {
        display->dis.mode = mode;
        return vesa_change_mode(display->dis.mode);
    }
    DEBUG("New mode for display 0x%x (Device: %s): %dx%dx%d (%s)\n", display, device->dev.dev.name, mode->width, mode->height, mode->depth, mode->type==CDI_VIDEO_TEXT?"Textmode":"Graph");
    display->dis.mode = mode;
    
    // Calculate offscreen buffer
    size_t screen_size = mode->width*mode->height*mode->depth/8;
    cirrus_vram_setmem(device,device->framebuf+screen_size,device->framebuf_size-screen_size);
    
    return 0;
}

/**
 * Returns a list with all possible modes for the display
 */
cdi_list_t cirrus_display_get_mode_list(struct cdi_video_display *dis)
{
    struct cirrus_display *display = (struct cirrus_display*)dis;
    return display->modes;
}

