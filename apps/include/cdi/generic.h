/*
 * Copyright (c) 2009 Janosch Gräf
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/projects/COPYING.WTFPL for more details.
 */

#ifndef _CDI_GENERIC_H_
#define _CDI_GENERIC_H_

#include <sys/types.h>

#include "cdi.h"
#include "cdi/lists.h"

/// Struktur fuer Stream-Objekt fuer Joystick
struct cdi_generic_stream_joystick {
  int axis_raw[2];
  int axis_cal[2];
  int buttons[2];
};

/**
 * Struktur fuer CDI Generic Driver
 */
struct cdi_generic_driver {
  /// Allgemeine CDI-Treiber-Struktur
  struct cdi_driver drv;
};

struct cdi_generic_stream;

/**
 * Struktur fuer CDI Generic Devices
 */
struct cdi_generic_device {
  /// Allgemeine CDI-Geraete-Struktur
  struct cdi_device dev;

  /// Datenstream des Geraets
  struct cdi_generic_stream *stream;

  /// Steuerfunktionen des Geraets
  cdi_list_t functions;
};

/**
 * Struktur eines Streams
 * Wie die Daten des Streams verarbeitet werden haengt vom Treiber ab.
 * Gebraeuchlich ist die Arbeitsweise FIFO
 *  - read() wird von der Implementation aufgerufen, wenn ein Programm Daten
 *    vom Stream lesen will.
 *  - write() wird von der Implementation aufgerufen, wenn ein Programm Daten
 *    in den Stream schreiben will.
 */
struct cdi_generic_stream {
  /// Groesse eines Objekts
  size_t objsz;
  /// Liest ein Objekt vom Stream
  int (*read)(struct cdi_generic_device *dev,void *obj,size_t count);
  /// Schreibt ein Objekt in den Stream
  int (*write)(struct cdi_generic_device *dev,void *obj,size_t count);
};

/**
 * Struktur einer Steuerfunktion
 * Steuerfunktionen werden vom Treiber zur Verfuegung gestellt um erweiterten
 * Zugriff auf das Geraet zu haben. Jede Funktion hat ein Objekt, dass der
 * Funktion vom Aufrufer uebergeben wird und ein Objekt, dass die Funktion
 * an den Aufrufer zurueck gibt.
 */
struct cdi_generic_function {
  /**
   * Referenz zur Steuerfunktion
   *  @param dev CDI-Geraet
   *  @param data Datenobjekt
   *  @param datasz Groesse von data
   *  @param ret Rueckgabeobjekt
   *  @param retsz Groesse von ret
   *  @return Rueckgabewert
   */
  int (*func)(struct cdi_generic_device *dev,void *data,size_t datasz,void *ret,size_t retsz);

  // meinOS specific
  /// Nummer der Funktion
  int cmd;
};

/**
 * Initialisiert einen CDI-Generic-Treiber
 *  @param driver CDI-Generic-Treiber
 */
void cdi_generic_driver_init(struct cdi_generic_driver* driver);

/**
 * Zerstoert einen CDI-Generic-Treiber
 *  @param driver CDI-Generic-Treiber
 */
void cdi_generic_driver_destroy(struct cdi_generic_driver* driver);

/**
 * Registriert einen CDI-Generic-Treiber
 *  @param driver CDI-Generic-Treiber
 */
void cdi_generic_driver_register(struct cdi_generic_driver* driver);

#endif
