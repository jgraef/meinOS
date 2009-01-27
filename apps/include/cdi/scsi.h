/*
 * Copyright (c) 2009 Janosch Gräf
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/projects/COPYING.WTFPL for more details.
 */

#ifndef _CDI_SCSI_H_
#define _CDI_SCSI_H_

#include <stdint.h>
#include <stddef.h>

#include "cdi.h"

/// SCSI-Paket
struct cdi_scsi_packet {
  /// Buffer zum Senden oder Empfangen von Daten
  void *buffer;

  /// Groesse des Buffers
  size_t bufsize;

  /// Ob gelesen oder geschrieben werden soll
  enum {
    CDI_SCSI_NODATA,
    CDI_SCSI_READ,
    CDI_SCSI_WRITE,
  } direction;

  /// SCSI Command
  uint8_t command[16];

  /// Groesse des SCSI Commands
  size_t cmdsize;
};

/// SCSI-Geraet
struct cdi_scsi_device {
  struct cdi_device dev;
};

/// SCSI-Treiber
struct cdi_scsi_driver {
  struct cdi_driver drv;

  /**
   * Sendet ein SCSI-Paket an das Geraet.
   *  @param device SCSI-Geraet
   *  @param packet Paket
   *  @return SCSI-Fehlerstatus nach der Ausfuehrung des Befehls
   */
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

/**
 * Initialisiert ein neues SCSI-Geraet
 *  @note Der Typ der Geraetes muss bereits gesetzt sein
 */
void cdi_scsi_device_init(struct cdi_scsi_device* device);

#endif
