/*
 * Copyright (c) 2008 Mathias Gottschlag and Janosch Graef
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it 
 * and/or modify it under the terms of the Do What The Fuck You Want 
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/projects/COPYING.WTFPL for more details.
 */

#ifndef _BITMAP_H_
#define _BITMAP_H_

#include <cdi/video.h>

#include "cirrus.h"

int cirrus_bitmap_upload(struct cdi_video_device *device, struct cirrus_bitmap *bitmap);
void cirrus_clean_vram(struct cdi_video_device *device);

struct cdi_video_bitmap *cirrus_bitmap_create(struct cdi_video_device *device, unsigned int width, unsigned int height, cdi_video_bitmap_format_t format, void *data);
void cirrus_bitmap_destroy(struct cdi_video_device *device, struct cdi_video_bitmap *bitmap);
void cirrus_bitmap_set_usage_hint(struct cdi_video_device *device, struct cdi_video_bitmap *bitmap, cdi_video_usage_hint_t hint);

#endif

