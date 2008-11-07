/*
 * Copyright (c) 2008 Mathias Gottschlag and Janosch Graef
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it 
 * and/or modify it under the terms of the Do What The Fuck You Want 
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/projects/COPYING.WTFPL for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cdi.h"
#include "cdi/misc.h"
#include "cdi/video.h"
#include "cdi/pci.h"
#include "cdi/lists.h"

#include "cirrus.h"
#include "vesa.h"
#include "draw.h"
#include "bitmap.h"

char *driver_name = "cirrus";
struct cdi_video_driver cirrus_driver;

static void cirrus_driver_init(void);
static void cirrus_driver_destroy(struct cdi_driver* driver);

#ifdef CDI_STANDALONE
int main(void)
#else
int init_cirrus(void)
#endif
{
    DEBUG("cirrus driver running\n");
    cdi_init();
    cirrus_driver_init();

    #ifdef CDI_STANDALONE
    cdi_run_drivers();
    #endif
    return 0;
}

/**
 * Initializes the cirrus driver and registers it
 */
static void cirrus_driver_init(void)
{
    // Abort at once if we don't have VESA
    if (vesa_initialize() == -1) {
        return;
    }

    cdi_list_t pci_devices;
    struct cdi_pci_device *pci;

    memset(&cirrus_driver, 0, sizeof(cirrus_driver));
    cdi_video_driver_init(&cirrus_driver);

    // Fill driver struct
    cirrus_driver.drv.name = driver_name;
    cirrus_driver.drv.type = CDI_VIDEO;
    cirrus_driver.drv.init_device = cirrus_device_init;
    cirrus_driver.drv.remove_device = cirrus_device_remove;
    cirrus_driver.drv.destroy = cirrus_driver_destroy;
    cirrus_driver.display_enable = cirrus_display_enable;
    cirrus_driver.display_disable = cirrus_display_disable;
    cirrus_driver.display_clear = cirrus_display_clear;
    cirrus_driver.display_set_mode = cirrus_display_set_mode;
    cirrus_driver.display_get_mode_list = cirrus_display_get_mode_list;
    /// @todo assignment from incompatible pointer type
    cirrus_driver.bitmap_create = cirrus_bitmap_create;
    /// @todo assignment from incompatible pointer type
    cirrus_driver.bitmap_destroy = cirrus_bitmap_destroy;
    /// @todo assignment from incompatible pointer type
    cirrus_driver.bitmap_set_usage_hint = cirrus_bitmap_set_usage_hint;
    cirrus_driver.set_raster_op = cirrus_set_raster_op;
    cirrus_driver.set_target = cirrus_set_target;
    cirrus_driver.draw_line = cirrus_draw_line;
    cirrus_driver.draw_rectangle = cirrus_draw_rectangle;
    cirrus_driver.draw_ellipse = cirrus_draw_ellipse;
    cirrus_driver.copy_screen = cirrus_copy_screen;
    cirrus_driver.draw_bitmap = cirrus_draw_bitmap;
    cirrus_driver.draw_bitmap_part = cirrus_draw_bitmap_part;
    /// @todo assignment from incompatible pointer type
    cirrus_driver.draw_dot = cirrus_draw_dot;
    TODO("Set rest of display handlers");

    // Find Cirrus cards
    pci_devices = cdi_list_create();
    cdi_pci_get_all_devices(pci_devices);
    while ((pci = cdi_list_pop(pci_devices))) {
        if (pci->vendor_id == CIRRUS_VENDORID && pci->device_id == CIRRUS_DEVICEID) {
            char *name = malloc(8);
            snprintf(name, 8, "cirrus%01d", cdi_list_size(cirrus_driver.drv.devices));
            struct cirrus_device *dev = cirrus_device_create(name, pci);
            dev->dev.dev.driver = (struct cdi_driver*)&cirrus_driver;
            cdi_list_push(cirrus_driver.drv.devices, dev);
        } else cdi_pci_device_destroy(pci);
    }
    cdi_list_destroy(pci_devices);

    // Registers the driver
    cdi_video_driver_register(&cirrus_driver);
}

/**
 * Destroys the cirrus driver
 * @param driver Pointer to the driver (unused)
 */
static void cirrus_driver_destroy(struct cdi_driver* driver)
{
    TODO("");
    cdi_video_driver_destroy((struct cdi_video_driver*)driver);
}
