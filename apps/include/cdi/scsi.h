/*
    meinOS - A unix-like x86 microkernel operating system
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _CDI_SCSI_H_
#define _CDI_SCSI_H_

#include <stdint.h>
#include <stddef.h>

#include "cdi.h"

// SCSI-Paket
struct cdi_scsi_packet {
  // Buffer zum Senden oder Empfangen von Daten
  void *buffer;

  // Groesse des Buffers
  size_t bufsize;

  // Ob gelesen oder geschrieben werden soll
  enum {
    CDI_SCSI_NODATA,
    CDI_SCSI_READ,
    CDI_SCSI_WRITE,
  } direction;

  // SCSI Command
  uint8_t command[16];

  // Groesse des SCSI Commands
  size_t cmdsize;
};

// SCSI-Geraet
struct cdi_scsi_device {
  struct cdi_device dev;
};

// SCSI-Driver
struct cdi_scsi_driver {
  struct cdi_driver drv;

  int (*request)(struct cdi_scsi_device *device,struct cdi_scsi_packet *packet);
};

/**
 * Ein SCSI-Paket allozieren
 *
 * @param size Benoetigte Groesse
 *
 * @return Pointer auf das Paket oder NULL im Fehlerfall
 */
struct cdi_scsi_packet* cdi_scsi_packet_alloc(size_t size);

/**
 * Ein SCSI-Paket freigeben
 *
 * @param packet Pointer auf das Paket
 */
void cdi_scsi_packet_free(struct cdi_scsi_packet* packet);

/**
 * Initialisiert die Datenstrukturen fuer einen SCSI-Treiber
 */
void cdi_scsi_driver_init(struct cdi_scsi_driver* driver);

/**
 * Deinitialisiert die Datenstrukturen fuer einen SCSI-Treiber
 */
void cdi_scsi_driver_destroy(struct cdi_scsi_driver* driver);

/**
 * Registiert einen SCSI-Treiber
 */
void cdi_scsi_driver_register(struct cdi_scsi_driver* driver);

#endif
