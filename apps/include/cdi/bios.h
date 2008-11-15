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

#ifndef _CDI_BIOS_H_
#define _CDI_BIOS_H_

#include <stdint.h>

#include "cdi/lists.h"

struct cdi_bios_registers {
  uint16_t ax;
  uint16_t bx;
  uint16_t cx;
  uint16_t dx;
  uint16_t si;
  uint16_t di;
  uint16_t ds;
  uint16_t es;
};

/**
 * Struktur zum Zugriff auf Speicherbereiche des 16bit-Prozesses
 */
struct cdi_bios_memory {
    /**
     * Virtuelle Addresse im Speicher des 16bit-Prozesses. Muss unterhalb von
     * 0xC0000 liegen.
     */
    uintptr_t dest;

    /**
     * Pointer auf reservierten Speicher für die Daten des Speicherbereichs. Wird
     * beim Start des Tasks zum Initialisieren des Bereichs benutzt und erhaelt
     * auch wieder die Daten nach dem BIOS-Aufruf.
     */
    void *src;

    /**
     * Laenge des Speicherbereichs
     */
    uint16_t size;
};

/**
 * Ruft einen BIOS-Interrupt auf.
 *
 * @param interrupt Nummer des BIOS-Interrupts
 * @param registers Pointer auf eine Register-Struktur. Diese wird beim Aufruf
 * in die Register des Tasks geladen und bei Beendigung des Tasks wieder mit den
 * Werten des Tasks gefuellt.
 * @param memory Speicherbereiche, die in den Bios-Task kopiert und bei Beendigung
 * des Tasks wieder zurueck kopiert werden sollen. Die Liste ist vom Typ struct
 * cdi_bios_memory.
 * @return 0, wenn der Aufruf erfolgreich war, -1 bei Fehlern
 */
int cdi_bios_int10(struct cdi_bios_registers *registers,cdi_list_t memory);

#endif
