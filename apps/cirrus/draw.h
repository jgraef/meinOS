/*
 * Copyright (c) 2008 Mathias Gottschlag and Janosch Graef
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it 
 * and/or modify it under the terms of the Do What The Fuck You Want 
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/projects/COPYING.WTFPL for more details.
 */

#ifndef _DRAW_H_
#define _DRAW_H_

#include "cdi/video.h"

void cirrus_set_raster_op(struct cdi_video_display *display, cdi_video_raster_op_t rop);

void cirrus_set_target(struct cdi_video_display *display, struct cdi_video_bitmap *bitmap);

void cirrus_draw_dot(struct cdi_video_display *display, unsigned int x, unsigned int y, int color);
void cirrus_draw_line(struct cdi_video_display *display, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, int color);
void cirrus_draw_rectangle(struct cdi_video_display *display, unsigned int x, unsigned int y, unsigned int width, unsigned int height, int color);
void cirrus_draw_ellipse(struct cdi_video_display *display, unsigned int x, unsigned int y, unsigned int width, unsigned int height, int color);

void cirrus_copy_screen(struct cdi_video_display *display, unsigned int x, unsigned int y, unsigned int srcx, unsigned int srcy, unsigned int width, unsigned int height);

void cirrus_draw_bitmap(struct cdi_video_display *display, struct cdi_video_bitmap *bitmap, unsigned int x, unsigned int y);

void cirrus_draw_bitmap_part(struct cdi_video_display *display, struct cdi_video_bitmap *bitmap, unsigned int x, unsigned int y, unsigned int srcx, unsigned int srcy, unsigned int width, unsigned int height);

#endif

