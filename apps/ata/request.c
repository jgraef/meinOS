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

#include "device.h"

/**
 * Warten bis ein ATA-Geraet bereit ist
 *
 * @param bits Bits die gesetzt sein muessen
 * @param timeout Timeout in Millisekunden
 *
 * @return 1 wenn das Geraet bereit ist, 0 sonst
 */
static inline int ata_drv_wait_ready(struct ata_device* dev, uint8_t bits,
    uint32_t timeout)
{
    struct ata_controller* ctrl = dev->controller;
    uint32_t time = 0;

    ATA_DELAY(dev->controller);

    // Zuerst warten, bis das Busy-Bit nicht meht gesetzt ist, denn erst dann
    // sind die anderen Bits gueltig
    while (((ata_reg_inb(ctrl, REG_STATUS) & STATUS_BSY)) &&
        (time < timeout))
    {
        time += 10;
        cdi_sleep_ms(10);
    }

    // Dem Geraet etwas Zeit geben
    ATA_DELAY(dev->controller);

    // Jetzt koennen wir warten bis die gewuenschten Bits gesetzt sind
    while (((ata_reg_inb(ctrl, REG_STATUS) & bits) != bits) &&
        (time < timeout))
    {
        time += 10;
        cdi_sleep_ms(10);
    }

    return (time < timeout);
}


/**
 * Befehlsausfuehrung von einem ATA-Request starten
 *
 * @return 1 wenn der Befehl erfolgreich gestartet wurde, 0 sonst
 */
static int ata_request_command(struct ata_request* request)
{
    struct ata_device* dev = request->dev;
    struct ata_controller* ctrl = dev->controller;
    uint8_t control;

    // IRQ-Zaehler zuruecksetzen, egal ob er gebraucht wird oder nicht, stoert
    // ja niemanden
    ctrl->irq_cnt = 0;

    ata_drv_select(dev);

    // Warten bis das Geraet bereit ist
    if (!ata_drv_wait_ready(dev, 0, ATA_READY_TIMEOUT)) {
        request->error = DEVICE_READY_TIMEOUT;
        return 0;
    }

    // Device Register schreiben
    // TODO: nicht lba?
    ata_reg_outb(ctrl, REG_DEVICE, (request->flags.lba << 6) | (request->dev->
        id << 4) | ((request->registers.ata.lba >> 24) & 0xF));

    // Control Register schreiben
    control = 0;
    if (request->flags.poll) {
        // Wenn nur gepollt werden soll, muessen Interrupts deaktiviert werden
        control |= CONTROL_NIEN;
    }
    // TODO: HOB
    ata_reg_outb(ctrl, REG_CONTROL, control);

    // Count-Register schrieben
    ata_reg_outb(ctrl, REG_SEC_CNT, request->registers.ata.count);

    // LBA Register schreiben
    ata_reg_outb(ctrl, REG_LBA_LOW, request->registers.ata.lba & 0xFF);
    ata_reg_outb(ctrl, REG_LBA_MID, (request->registers.ata.lba >> 8) & 0xFF);
    ata_reg_outb(ctrl, REG_LBA_HIG, (request->registers.ata.lba >> 16) &
        0xFF);

    // Befehl ausfuehren
    ata_reg_outb(ctrl, REG_COMMAND, request->registers.ata.command);

    return 1;
}

/**
 * Verarbeitet einen ATA-Request bei dem keine Daten uebertragen werden
 */
static int ata_protocol_non_data(struct ata_request* request)
{
    struct ata_device* dev = request->dev;
    struct ata_controller* ctrl = dev->controller;

    // Aktueller Status im Protokol
    enum {
        IRQ_WAIT,
        CHECK_STATUS
    } state;

    // Der Anfangsstatus haengt davon ab, ob gepollt werden soll oder nicht.
    if  (request->flags.poll) {
        state = CHECK_STATUS;
    } else {
        state = IRQ_WAIT;
    }

    while (1) {
        switch (state) {
            case IRQ_WAIT:
                // Auf IRQ warten
                if (ata_wait_irq(ctrl, ATA_IRQ_TIMEOUT)) {
                    request->error = IRQ_TIMEOUT;
                    DEBUG("non_data IRQ-Timeout\n");
                    return 0;
                }

                // Jetzt muss der Status ueberprueft werden
                state = CHECK_STATUS;
                break;

            case CHECK_STATUS: {
                uint8_t status = ata_reg_inb(ctrl, REG_STATUS);

                // Status ueberpruefen
                if ((status & STATUS_BSY) == STATUS_BSY) {
                    // Wenn das Busy-Flag gesetzt ist, muss gewartet werden,
                    // bis es geloescht wird.
                    cdi_sleep_ms(20);
                } else {
                    return 1;
                }
                break;
            }
        }
    }
}

/**
 * Verarbeitet einen ATA-Request bei dem Daten ueber PIO eingelesen werden
 * sollen.
 * Nach Kapitel 11 in der ATA7-Spec
 */
int ata_protocol_pio_in(struct ata_request* request)
{
    struct ata_device* dev = request->dev;
    struct ata_controller* ctrl = dev->controller;
    size_t packet_size = 0;

    // Aktueller Status im Protokol
    enum {
        IRQ_WAIT,
        CHECK_STATUS,
        TRANSFER_DATA
    } state;

    // Der Anfangsstatus haengt davon ab, ob gepollt werden soll oder nicht.
    if  (request->flags.poll) {
        state = CHECK_STATUS;
    } else {
        state = IRQ_WAIT;
    }

    while (1) {
        switch (state) {
            case IRQ_WAIT:
                // Auf IRQ warten
                if (ata_wait_irq(ctrl, ATA_IRQ_TIMEOUT)) {
                    request->error = IRQ_TIMEOUT;
                    DEBUG("pio_in IRQ-Timeout\n");
                    return 0;
                }

                if (request->flags.ata && packet_size==0) // ATAPI
                {
                    // Paketgroesse einlesen, da sonst unendlich viel gelesen wird
                    packet_size = ata_reg_inb(ctrl,REG_LBA_MID)|(ata_reg_inb(ctrl,REG_LBA_HIG)<<8);
                }

                // Jetzt muss der Status ueberprueft werden
                state = CHECK_STATUS;
                break;

            case CHECK_STATUS: {
                uint8_t status = ata_reg_inb(ctrl, REG_STATUS);

                // Status ueberpruefen
                // Wenn DRQ und BSY geloescht wurden ist irgendetwas schief
                // gelaufen.
                if ((status & (STATUS_BSY | STATUS_DRQ)) == 0) {
                    // TODO: Fehlerbehandlung
                    DEBUG("pio_in unerwarteter Status: 0x%x\n", status);
                    return 0;
                } else if ((status & STATUS_BSY) == STATUS_BSY) {
                    // Wenn das Busy-Flag gesetzt ist, muss gewartet werden,
                    // bis es geloescht wird.
                    ATA_DELAY(ctrl);
                } else if ((status & (STATUS_BSY | STATUS_DRQ)) == STATUS_DRQ)
                {
                    // Wenn nur DRQ gesetzt ist, sind Daten bereit um abgeholt
                    // zu werden. Transaktion nach Transfer Data
                    state = TRANSFER_DATA;
                }
                break;
            }

            case TRANSFER_DATA: {
                uint16_t* buffer = (uint16_t*) (request->buffer + (request->
                    blocks_done * request->block_size));

                ata_insw(ata_reg_base(ctrl, REG_DATA) + REG_DATA, buffer,
                    request->block_size / 2);

                // Anzahl der gelesenen Block erhoehen
                request->blocks_done++;

                // Naechste Transaktion ausfindig machen
                if (!request->flags.ata &&
                    request->blocks_done >= request->block_count) {
                    // Wenn alle Blocks gelesen wurden ist der Transfer
                    // abgeschlossen.
                    return 1;
                } else if (request->flags.ata &&
                         request->blocks_done*request->block_size>=packet_size)
                {
                    // Wenn alle Bytes des ATAPI-Paketes gelesen wurden
                    // ist der Transfer abgeschlossen
                    return 1;
                } else if (request->flags.poll) {
                    // Wenn gepollt wird, muss jetzt gewartet werden, bis der
                    // Status wieder stimmt um den naechsten Block zu lesen.
                    state = CHECK_STATUS;
                } else {
                    // Bei der Benutzung von Interrupts wird jetzt auf den
                    // naechsten Interrupt gewartet
                    state = IRQ_WAIT;
                }
            }
        }
    }
}

/**
 * Verarbeitet einen ATA-Request bei dem Daten ueber PIO geschrieben werden
 * sollen
 */
int ata_protocol_pio_out(struct ata_request* request)
{
    struct ata_device* dev = request->dev;
    struct ata_controller* ctrl = dev->controller;
    size_t packet_size = 0;

    // Aktueller Status im Protokol
    enum {
        IRQ_WAIT,
        CHECK_STATUS,
        TRANSFER_DATA
    } state;

    state = CHECK_STATUS;
    while (1) {
        switch (state) {
            case IRQ_WAIT:
                // Auf IRQ warten
                if (ata_wait_irq(ctrl, ATA_IRQ_TIMEOUT)) {
                    request->error = IRQ_TIMEOUT;
                    DEBUG("pio_out IRQ-Timeout\n");
                    return 0;
                }

                if (request->flags.ata && packet_size==0) // ATAPI
                {
                    // Paketgroesse einlesen, da sonst unendlich viel geschrieben wird
                    packet_size = ata_reg_inb(ctrl,REG_LBA_MID)|(ata_reg_inb(ctrl,REG_LBA_HIG)<<8);
                    DEBUG("packet_size = %d\n",packet_size);
                }

                // Jetzt muss der Status ueberprueft werden
                state = CHECK_STATUS;
                break;

            case CHECK_STATUS: {
                uint8_t status = ata_reg_inb(ctrl, REG_STATUS);

                if (request->flags.ata &&
                    request->blocks_done * request->block_size>=packet_size)
                {
                    // Das Paket wurde vollstaendig gelesen. DRQ wird nicht
                    // gesetzt, deswegen muss so beendet werden.
                    return 1;
                }
                else if (!request->flags.ata &&
                         request->blocks_done==request->block_count)
                {
                    // Der Buffer wurde komplett gelesen. Dies sollte bei
                    // ATAPI nicht passieren!
                    return 1;
                }
                else if ((status & (STATUS_BSY | STATUS_DRQ)) == 0)
                {
                    // TODO: Fehlerbehandlung
                    DEBUG("pio_out unerwarteter Status: 0x%x\n",
                           status);
                    return 0;

                } else if ((status & STATUS_BSY) == STATUS_BSY) {
                    // Wenn das Busy-Flag gesetzt ist, muss gewartet werden,
                    // bis es geloescht wird.
                    cdi_sleep_ms(20);
                } else if ((status & (STATUS_BSY | STATUS_DRQ)) == STATUS_DRQ)
                {
                    // Wenn nur DRQ gesetzt ist, ist der Kontroller bereit um
                    // Daten zu empfangen.
                    // Transaktion nach Transfer Data
                    state = TRANSFER_DATA;
                }
                break;
            }

            case TRANSFER_DATA: {
                uint16_t i;
                uint16_t* buffer = (uint16_t*) (request->buffer + (request->
                    blocks_done * request->block_size));

                // Einen Block schreiben
                for (i = 0; i < request->block_size / 2; i++) {
                    ata_reg_outw(ctrl, REG_DATA, buffer[i]);
                }

                // Anzahl der geschriebenen Block erhoehen
                request->blocks_done++;

                // Naechste Transaktion ausfindig machen
                if (request->flags.poll) {
                    // Wenn gepollt wird, muss jetzt gewartet werden, bis der
                    // Status wieder stimmt um den naechsten Block zu
                    // schreiben.
                    state = CHECK_STATUS;
                } else {
                    // Bei der Benutzung von Interrupts wird jetzt auf den
                    // naechsten Interrupt gewartet
                    state = IRQ_WAIT;
                }
            }
        }
    }
}

/**
 * Fuehrt einen ATA-Request aus.
 *
 * @return 1 Wenn der Request erfolgreich bearbeitet wurde, 0 sonst
 */
int ata_request(struct ata_request* request)
{
   // printf("ata: [%d:%d] Request command=%x count=%x lba=%llx protocol=%x\n", request->dev->controller->id, request->dev->id, request->registers.ata.command, request->registers.ata.count, request->registers.ata.lba, request->protocol);
    // Befehl ausfuehren
    if (!ata_request_command(request)) {
        DEBUG("Fehler bei der Befehlsausfuehrung\n");
        return 0;
    }

    // Je nach Protokoll werden jetzt die Daten uebertragen
    switch (request->protocol) {
        case NON_DATA:
            if (!ata_protocol_non_data(request)) {
                return 0;
            }
            break;

        case PIO:
            if ((request->flags.direction == READ) &&
                (!ata_protocol_pio_in(request)))
            {
                return 0;
            } else if ((request->flags.direction == WRITE) &&
                (!ata_protocol_pio_out(request)))
            {
                return 0;
            }
            break;
    }
    return 1;
}

