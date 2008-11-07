/*
 * Copyright (c) 2008 Mathias Gottschlag and Janosch Graef
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it 
 * and/or modify it under the terms of the Do What The Fuck You Want 
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/projects/COPYING.WTFPL for more details.
 */

#include "bitmap.h"
#include "cirrus.h"

#include <stdlib.h>

// TODO: Depdendent on resolution
static int vramoffset = 1024 * 768 * 3;

static void convert1to24(char *src, char *dest, int pixelcount)
{
    int i;
    for (i = 0; i < pixelcount; i++) {
        if (src[i / 8] & (0x80 >> (i % 8))) {
	    *(dest++) = 0xFF;
	    *(dest++) = 0xFF;
	    *(dest++) = 0xFF;
        } else {
	    *(dest++) = 0x00;
	    *(dest++) = 0x00;
	    *(dest++) = 0x00;
        }
    }
}

struct cdi_video_bitmap *cirrus_bitmap_create(struct cdi_video_device *device, unsigned int width, unsigned int height, cdi_video_bitmap_format_t format, void *data)
{
    struct cirrus_bitmap *newbitmap = malloc(sizeof(struct cirrus_bitmap));
    newbitmap->bitmap.width = width;
    newbitmap->bitmap.height = height;
    newbitmap->bitmap.format = format;
    newbitmap->bitmap.pixeldata = data;
    newbitmap->vramaddr = 0;
    ((struct cirrus_device*)device)->bitmaps = cdi_list_push(((struct cirrus_device*)device)->bitmaps, newbitmap);
    // TODO: Upload texture?
    return &newbitmap->bitmap;
}
int cirrus_bitmap_upload(struct cdi_video_device *device, struct cirrus_bitmap *bitmap)
{
    struct cirrus_display *display = ((struct cirrus_device*)device)->display;
    int bitmapoffset = vramoffset;
    vramoffset += bitmap->bitmap.width * bitmap->bitmap.height * display->dis.mode->depth / 8;
    bitmap->vramaddr = bitmapoffset;
    switch (bitmap->bitmap.format) {
        case CDI_VIDEO_FMT_MONO_1:
            if (display->dis.mode->depth == 24) {
                convert1to24((char*)bitmap->bitmap.pixeldata, (char*)((struct cirrus_device*)device)->framebuf + bitmapoffset, bitmap->bitmap.width * bitmap->bitmap.height);
                return 1;
            }
            break;
        default:
            break;
    }
    return 0;
}

void cirrus_bitmap_destroy(struct cdi_video_device *device, struct cdi_video_bitmap *bitmap)
{
    TODO("");
}
void cirrus_bitmap_set_usage_hint(struct cdi_video_device *device, struct cdi_video_bitmap *bitmap, cdi_video_usage_hint_t hint)
{
    TODO("");
}

