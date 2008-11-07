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
