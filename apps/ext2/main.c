/*
 * Copyright (c) 2008 The LOST Project. All rights reserved.
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

#include "cdi/fs.h"
#include "cdi/misc.h"

#include "ext2_cdi.h"

struct ext2_driver {
    struct cdi_fs_driver fs;
};

static struct ext2_driver ext2_driver;
static const char* driver_name = "ext2";

static int ext2_driver_init(struct ext2_driver* driver);
static void ext2_driver_destroy(struct cdi_driver* driver);

#ifdef CDI_STANDALONE
int main()
#else
int init_ext2
#endif
{
    cdi_init();

    if (ext2_driver_init(&ext2_driver) != 0) {
        return -1;
    }
    cdi_fs_driver_register((struct cdi_fs_driver*) &ext2_driver);

#ifdef CDI_STANDALONE
    cdi_run_drivers();
#endif

    return 0;
}

/**
 * Initialisiert die Datenstrukturen fuer den ext2-Treiber
 */
static int ext2_driver_init(struct ext2_driver* driver)
{
    // Konstruktor der Vaterklasse
    cdi_fs_driver_init((struct cdi_fs_driver*) driver);

    // Namen setzen
    driver->fs.drv.name = driver_name;
    driver->fs.fs_init = ext2_fs_init;
    driver->fs.fs_destroy = ext2_fs_destroy;

    driver->fs.drv.destroy = ext2_driver_destroy;
    return 0;
}

/**
 * Deinitialisiert die Datenstrukturen fuer den sis900-Treiber
 */
static void ext2_driver_destroy(struct cdi_driver* driver)
{
    cdi_fs_driver_destroy((struct cdi_fs_driver*) driver);
}
