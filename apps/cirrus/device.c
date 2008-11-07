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
#include "cdi/pci.h"
#include "cdi/video.h"
#include "cdi/misc.h"

#include "cirrus.h"

/**
 * Creates an Cirrus device
 *  @param name Device name
 *  @param pci CDI PCI object
 *  @return Cirrus device
 */
struct cirrus_device *cirrus_device_create(const char *name, struct cdi_pci_device *pci)
{
    struct cirrus_device *dev = malloc(sizeof(struct cirrus_device));

    DEBUG("New device %s: pci.%d.%d.%d\n", name, pci->bus, pci->dev, pci->function);

    // Fill device struct
    dev->dev.dev.name = name;
    dev->dev.dev.type = CDI_VIDEO;
    dev->dev.displays = cdi_list_create();
    dev->pci = pci;
    dev->display = cirrus_display_create();
    dev->display->dis.device = (struct cdi_video_device*)dev;
    dev->framebuf = NULL;
    dev->framebuf_size = 0;
    dev->regs = NULL;
    dev->bitmaps = cdi_list_create();

    cdi_list_push(dev->dev.displays,dev->display);

    return dev;
}

/**
 * Initializes a Cirrus device
 */
void cirrus_device_init(struct cdi_device *dev)
{
    struct cirrus_device *device = (struct cirrus_device*)dev;
    struct cdi_pci_resource *res;
    size_t i;
    
    // Allocate resources
    cdi_pci_alloc_memory(device->pci);
  
    for (i=0; (res = cdi_list_get(device->pci->resources, i)); i++) {
        if (res->index == 0) {
            // This resource is the frame buffer
            device->framebuf = res->address;
            TODO("FIXME: Kann man die Groesse herausfinden, ohne eine feste Zahl zu benutzen?");
            device->framebuf_size = 4*1024*1024;//res->length;
            DEBUG("%s: Framebuffer size: %dkB\n",device->dev.dev.name,res->length/1024);
        } else {
            // This resource is the register region
            // res->length wird nicht benoetigt
            device->regs = res->address;
        }
    }
    
    if (device->framebuf == NULL) {
        printf("Could not allocate frame buffer for device %s\n", device->dev.dev.name);
    }
    if (device->regs == NULL) {
        printf("Could not allocate registers for device %s\n", device->dev.dev.name);
    }
    
    // Enable acceleration
    if (device->regs) {
        device->regs->graph_index = 0x0E;
        device->regs->graph_data = 0x20;
    }
}

void cirrus_device_remove(struct cdi_device *dev) {
    struct cirrus_device *device = (struct cirrus_device*)dev;

    free((char*)device->dev.dev.name);
    cdi_pci_device_destroy(device->pci);
    // TODO: Delete bitmaps
    cdi_list_destroy(device->bitmaps);
}
