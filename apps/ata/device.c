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

#include <stdio.h>
#include <stdlib.h>

#include "cdi.h"
#include "cdi/storage.h"
#include "cdi/misc.h"
#include "cdi/io.h"
#include "cdi/scsi.h"

#include "device.h"
#include "libpartition.h"

// IRQ-Handler
static inline void ata_controller_irq(struct cdi_device* dev);

/**
 * Diese Funktion prueft, ob der Bus leer ist. Denn wenn der Bus leer ist, wird
 * jeder Versuch das Status-Register zu lesen 0xFF zurueck geben, was ein
 * ungueltiger Wert ist. Das haengt damit zusammen, dass der Bus mit
 * Pullup-Widerstaenden auf hohem Pegel gehalten wird, wenn keine Geraete
 * angeschlossen sind.
 *
 * @return 1 wenn der Bus leer ist, 0 sonst
 */
static int ata_bus_floating(struct ata_controller* controller)
{
    uint8_t status;

    // Master auswaehlen und Status auslesen
    ata_reg_outb(controller, REG_DEVICE, DEVICE_DEV(0));
    ATA_DELAY(controller);
    status = ata_reg_inb(controller, REG_STATUS);

    // Nicht floating
    if (status != 0xFF) {
        return 0;
    }

    // Slave auswaehlen
    ata_reg_outb(controller, REG_DEVICE, DEVICE_DEV(1));
    ATA_DELAY(controller);
    status = ata_reg_inb(controller, REG_STATUS);

    // Wenn alle Bits gesetzt sind, ist der Bus floating
    return status==0xFF;
}

/**
 * Diese Funktion prueft, ob mindestens ein Antwortendes Geraet am Bus haengt.
 * Das laesst sich herausfinden, indem irgendwelche Werte in die LBA-Register
 * geschrieben werden. Wenn ein Geraet vorhanden ist, muessen die Werte wieder
 * ausgelesen werden koennen.
 *
 * @return 1 wenn ein Geraet vorhanden ist, 0 sonst
 */
static int ata_bus_responsive_drv(struct ata_controller* controller)
{
    // Slave auswaehlen, da so sicher jemand reagiert, da der Master antworten
    // muss, wenn kein Slave existiert. So sieht es zumindest in der Theorie
    // aus. In der Praxis scheint vmware das nicht so zu machen. Deshalb drehen
    // wir weiter unten noch eine Sonderrunde fuer vmware. ;-)
    ata_reg_outb(controller, REG_DEVICE, DEVICE_DEV(1));
    ATA_DELAY(controller);

    // Jetzt wird irgendwas auf die Ports geschrieben. Die Werte sind absolut
    // egal, solange das selbe beim auslesen zurueckkommt ist gut.
    ata_reg_outb(controller, REG_LBA_LOW, 0xAF);
    ata_reg_outb(controller, REG_LBA_MID, 0xBF);
    ata_reg_outb(controller, REG_LBA_HIG, 0xCF);

    // Wenn die ausgelesenen Werte stimmen ist mindestens ein Geraet vorhanden
    if ((ata_reg_inb(controller, REG_LBA_LOW) == 0xAF) &&
        (ata_reg_inb(controller, REG_LBA_MID) == 0xBF) &&
        (ata_reg_inb(controller, REG_LBA_HIG) == 0xCF))
    {
        return 1;
    }



    // Hier noch die kleine Ehrenrunde fuer vmware ;-)
    ata_reg_outb(controller, REG_DEVICE, DEVICE_DEV(0));
    ATA_DELAY(controller);

    // Jetzt wird irgendwas auf die Ports geschrieben. Die Werte sind absolut
    // egal, solange das selbe beim auslesen zurueckkommt ist gut.
    ata_reg_outb(controller, REG_LBA_LOW, 0xAF);
    ata_reg_outb(controller, REG_LBA_MID, 0xBF);
    ata_reg_outb(controller, REG_LBA_HIG, 0xCF);

    // Wenn die ausgelesenen Werte stimmen ist mindestens ein Geraet vorhanden
    if ((ata_reg_inb(controller, REG_LBA_LOW) == 0xAF) &&
        (ata_reg_inb(controller, REG_LBA_MID) == 0xBF) &&
        (ata_reg_inb(controller, REG_LBA_HIG) == 0xCF))
    {
        return 1;
    }

    return 0;
}

/**
 * IRQ-Handler fuer ATA-Controller
 *
 * @param dev Achtung hier handelt es sich um kein echtes Geraet!!! Das wurde
 *            nur zum Registrieren des Handlers angelegt.
 */
static void ata_controller_irq(struct cdi_device* dev)
{
    struct ata_device* ata_dev = (struct ata_device*) dev;
    struct ata_controller* ctrl = ata_dev->controller;

    ctrl->irq_cnt++;
}

/**
 * Auf einen IRQ warten
 *
 * @param timeout Timeout nachdem die Funktion abbricht
 *
 * @return 1 Wenn der IRQ wie erwartet aufgetreten ist, 0 sonst
 */
int ata_wait_irq(struct ata_controller* controller, uint32_t timeout)
{
    uint32_t time = 0;

    // Warten bis der IRQ-Zaehler vom Handler erhoeht wird
    while (controller->irq_cnt == 0) {
        cdi_sleep_ms(20);
        time += 20;

        if (timeout <= time) {
            return 0;
        }
    }

    return 1;
}

/**
 * Partitionstabelle auf einem ATA-Geraet verarbeiten
 */
void ata_parse_partitions(struct ata_device* dev)
{
    struct partition_table partition_table;
    uint8_t mbr[ATA_SECTOR_SIZE];
    int i;

    // Die Partitionstabelle liegt im MBR
    if (!dev->read_sectors(dev, 0, 1, mbr)) {
        DEBUG("Fehler beim Einlesen der Partitionstabelle");
        return;
    }

    // Partitionstabelle Verarbeiten
    if (!partition_table_fill(&partition_table, mbr)) {
        DEBUG("Fehler beim Verarbeiten der Partitionstabelle");
        return;
    }

    // Ansonsten wird die Tabelle jetzt verarbeitet
    for (i = 0; i < 4; i++) {
        if (partition_table.entries[i].used) {
            // Erweiterter Eintrag => ueberspringen
            if (partition_table.entries[i].type == PARTITION_TYPE_EXTENDED) {
                DEBUG("TODO Erweiterte Partitionstabelleneintraege\n");
                continue;
            }

            struct ata_partition* partition = malloc(sizeof(*partition));
            partition->realdev = dev;
            partition->null = NULL;
            partition->start = partition_table.entries[i].start;
            partition->dev.storage.block_size = ATA_SECTOR_SIZE;
            partition->dev.storage.block_count = partition_table.entries[i].size;
            DEBUG("Partition von %d bis %d\n", partition->start, partition->dev.storage.block_count + partition->start - 1);
            asprintf((char**) &(partition->dev.storage.dev.name), "ata%01d%01d_p%01d",
                (uint32_t) dev->controller->id, dev->id, cdi_list_size(dev->
                partition_list));

            // Geraet registrieren
            cdi_list_push(dev->controller->storage->drv.devices, partition);

            // An Partitionsliste fuer das aktuelle Geraet anhaengen
            cdi_list_push(dev->partition_list, partition);
        }
    }
}




/**
 * ATA-Geraet initialisieren
 */
void ata_init_controller(struct ata_controller* controller)
{
    int i;

    // Ports registrieren
    if (cdi_ioports_alloc(controller->port_cmd_base, 8) != 0) {
        DEBUG("Fehler beim allozieren der I/O-Ports\n");
        return;
    }
    if (cdi_ioports_alloc(controller->port_ctl_base, 1) != 0) {
        DEBUG("Fehler beim allozieren der I/O-Ports\n");
        cdi_ioports_free(controller->port_cmd_base, 8);
        return;
    }

    // Da NIEN nicht ueberall sauber funktioniert, muss jetzt trotzdem schon
    // ein IRQ-Handler registriert werden. Und dafuer brauchen wir nun mal ein
    // Geraet.
    controller->irq_dev.controller = controller;
    cdi_register_irq(controller->irq, ata_controller_irq, (struct cdi_device*)
        &controller->irq_dev);

    // Laut osdev-Forum sollte das HOB-Bit fuer alle Geraete geloescht werden,
    // da noch nicht sicher ist, ob diese LBA48 unterstuetzen. Gleichzeitig
    // werden auch noch interrupts deaktiviert, da die erst benutzt werden
    // koennen, wenn feststeht, welche geraete existieren.
    //
    // ACHTUNG: NIEN scheint nicht ueberall zu funktionieren!! Auf einem meiner
    // Testrechner kommen da Trotzdem Interrupts.
    for (i = 0; i < 2; i++) {
        // Geraet auswaehlen
        ata_reg_outb(controller, REG_DEVICE, DEVICE_DEV(i));
        ATA_DELAY(controller);

        // HOB loeschen und NIEN setzen
        ata_reg_outb(controller, REG_CONTROL, CONTROL_NIEN);
    }

    // Jetzt pruefen wir ob der Bus leer ist (mehr dazu bei ata_bus_floating).
    if (ata_bus_floating(controller)) {
        DEBUG("Floating Bus %d\n", controller->id);
        return;
    }

    // Testen ob mindestens ein antwortendes Geraet am Bus haengt. Hier muss
    // darauf geachtet werden, dass der Master auch antworten muss, wenn zwar
    // der Slave ausgewaehlt ist, aber nicht existiert.
    if (!ata_bus_responsive_drv(controller)) {
        DEBUG("No responsive drive on Bus %d\n", controller->id);
        return;
    }

    // Jetzt werden die einzelnen geraete identifiziert. Hier ist es
    // anscheinend geschickter mit dem Slave zu beginnen.
    for (i = 1; i >= 0; i--) {
        struct ata_device* dev = malloc(sizeof(*dev));
        dev->controller = controller;
        dev->id = i;
        dev->partition_list = cdi_list_create();

        if (ata_drv_identify(dev)) {
            DEBUG("Bus %d  Device %d: ATAPI=%d\n", (uint32_t) controller->id, i, dev->atapi);

#ifdef ATAPI_ENABLE
            if (dev->atapi == 0) {
#endif
                dev->dev.storage.block_size = ATA_SECTOR_SIZE;

                // Handler setzen
                dev->read_sectors = ata_drv_read_sectors;
                dev->write_sectors = ata_drv_write_sectors;

                // Name setzen
                asprintf((char**) &(dev->dev.storage.dev.name), "ata%01d%01d",
                    (uint32_t) controller->id, i);

                // Partitionen parsen
                ata_parse_partitions(dev);

                // Geraet registrieren
                cdi_list_push(controller->storage->drv.devices, dev);
#ifdef ATAPI_ENABLE
            } else {
                // Name setzen
                asprintf((char**) &(dev->dev.scsi.dev.name),"atapi%01d%01d",
                    (uint32_t) controller->id, i);

                // Geraet registrieren
                cdi_list_push(controller->scsi->drv.devices, dev);
            }
#endif
        } else {
            free(dev);
        }
    }
}

void ata_remove_controller(struct ata_controller* controller)
{

}


void ata_init_device(struct cdi_device* device)
{
}

void ata_remove_device(struct cdi_device* device)
{

}


/**
 * Blocks von einem ATA(PI) Geraet lesen
 */
int ata_read_blocks(struct cdi_storage_device* device, uint64_t block,
    uint64_t count, void* buffer)
{
    struct ata_device* dev = (struct ata_device*) device;
    struct ata_partition* partition = NULL;

    // Wenn der Pointer auf den Controller NULL ist, handelt es sich um eine
    // Partition
    if (dev->controller == NULL) {
        partition = (struct ata_partition*) dev;
        dev = partition->realdev;
    }

    // Natuerlich nur ausfuehren wenn ein Handler eingetragen ist
    if (dev->read_sectors == NULL) {
        return -1;
    }

    // Bei einer Partition noch den Offset dazurechnen
    if (partition == NULL) {
        return !dev->read_sectors(dev, block, count, buffer);
    } else {
        return !dev->read_sectors(dev, block + partition->start, count,
            buffer);
    }
}

/**
 * Blocks auf ein ATA(PI) Geraet schreiben
 */
int ata_write_blocks(struct cdi_storage_device* device, uint64_t block,
    uint64_t count, void* buffer)
{
    struct ata_device* dev = (struct ata_device*) device;
    struct ata_partition* partition = NULL;

    // Wenn der Pointer auf den Controller NULL ist, handelt es sich um eine
    // Partition
    if (dev->controller == NULL) {
        partition = (struct ata_partition*) dev;
        dev = partition->realdev;
    }

    // Natuerlich nur ausfuehren wenn ein Handler eingetragen ist
    if (dev->write_sectors == NULL) {
        return -1;
    }

    // Bei einer Partition muss noch ein Offset drauf addiert werden
    if (partition == NULL) {
        return !dev->write_sectors(dev, block, count, buffer);
    } else {
        return !dev->write_sectors(dev, block + partition->start, count,
            buffer);
    }
}

