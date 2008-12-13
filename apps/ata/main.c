/*
 * Copyright (c) 2007 The LOST Project. All rights reserved.
 *
 * This code is derived from software contributed to the LOST Project
 * by Antoine Kaufmann.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *     This product includes software developed by the LOST Project
 *     and its contributors.
 * 4. Neither the name of the LOST Project nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "cdi/storage.h"
#include "cdi/lists.h"

#include "device.h"

static struct cdi_storage_driver driver_storage;
static struct cdi_scsi_driver driver_scsi;
static const char* driver_storage_name = "ata";
static const char* driver_scsi_name = "atapi";
static cdi_list_t controller_list = NULL;

static void ata_driver_init(void);
static void ata_driver_destroy(struct cdi_driver* driver);
static void atapi_driver_destroy(struct cdi_driver* driver);

#ifdef CDI_STANDALONE
int main(void)
#else
int init_ata(void)
#endif
{
    cdi_init();
    ata_driver_init();
    cdi_storage_driver_register((struct cdi_storage_driver*) &driver_storage);
    cdi_scsi_driver_register((struct cdi_scsi_driver*) &driver_scsi);

#ifdef CDI_STANDALONE
    cdi_run_drivers();
#endif

    return 0;
}

/**
 * Initialisiert die Datenstrukturen fuer den sis900-Treiber
 */
static void ata_driver_init()
{
    struct ata_controller* controller;

    // Konstruktor der Vaterklasse
    cdi_storage_driver_init((struct cdi_storage_driver*) &driver_storage);
    cdi_scsi_driver_init((struct cdi_scsi_driver*) &driver_scsi);

    // Namen setzen
    driver_storage.drv.name = driver_storage_name;
    driver_scsi.drv.name = driver_scsi_name;

    // Funktionspointer initialisieren
    driver_storage.drv.destroy          = ata_driver_destroy;
    driver_storage.drv.init_device      = ata_init_device;
    driver_storage.drv.remove_device    = ata_remove_device;
    driver_storage.read_blocks          = ata_read_blocks;
    driver_storage.write_blocks         = ata_write_blocks;

    driver_scsi.drv.destroy             = atapi_driver_destroy;
    driver_scsi.drv.init_device         = atapi_init_device;
    driver_scsi.drv.remove_device       = atapi_remove_device;
    driver_scsi.request                 = atapi_request;

    // Liste mit Controllern initialisieren
    controller_list = cdi_list_create();

    // Primaeren Controller vorbereiten
    controller = malloc(sizeof(*controller));
    controller->port_cmd_base = ATA_PRIMARY_CMD_BASE;
    controller->port_ctl_base = ATA_PRIMARY_CTL_BASE;
    controller->irq = ATA_PRIMARY_IRQ;
    controller->id = 0;
    controller->storage = (struct cdi_storage_driver*) &driver_storage;
    controller->scsi = (struct cdi_scsi_driver*) &driver_scsi;
    ata_init_controller(controller);
    cdi_list_push(controller_list, controller);

    // Sekundaeren Controller vorbereiten
    controller = malloc(sizeof(*controller));
    controller->port_cmd_base = ATA_SECONDARY_CMD_BASE;
    controller->port_ctl_base = ATA_SECONDARY_CTL_BASE;
    controller->irq = ATA_SECONDARY_IRQ;
    controller->id = 1;
    controller->storage = (struct cdi_storage_driver*) &driver_storage;
    controller->scsi = (struct cdi_scsi_driver*) &driver_scsi;
    ata_init_controller(controller);
    cdi_list_push(controller_list, controller);
}

/**
 * Deinitialisiert die Datenstrukturen fuer den ata-Treiber
 */
static void ata_driver_destroy(struct cdi_driver* driver)
{
    cdi_storage_driver_destroy((struct cdi_storage_driver*) driver);

    // TODO Alle Karten deinitialisieren
}

static void atapi_driver_destroy(struct cdi_driver* driver)
{
    cdi_scsi_driver_destroy((struct cdi_scsi_driver*) driver);
}

void debug(const char *fmt,...) {
#ifdef DEBUG
  va_list args;
  va_start(args,fmt);
  fprintf(DEBUG,"ata: ");
  vfprintf(DEBUG,fmt,args);
  va_end(args);
#endif
}
