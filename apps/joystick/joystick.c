/*
    meinOS - A unix-like x86 microkernel operating system
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <sys/types.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>

#include "cdi/generic.h"
#include "cdi/misc.h"
#include "cdi/io.h"

#define JOYSTICK_PORT     0x201
#define JOYSTICK_AXIS_MAX 32766
#define JOYSTICK_AXIS_MIN (-32766)

/// Struktur einer Achse
struct axis {
  /// Rohwert
  unsigned int raw;
  /// Minimaler Rohwert
  int min;
  int low_scale;
  /// Normaler Rohwert
  int norm;
  /// Maximaler Rohwert
  int max;
  int high_scale;
  /// Kalibrierter Wert
  int calibrated;
};

struct joystick {
  struct cdi_generic_device dev;
  int num;
  struct axis axis[2];
  int buttons[2];
};

struct joystick joysticks[2];
struct cdi_generic_driver *joystick_driver;

void joystick_raw2cal(struct axis *axis) {
  if (axis->min!=axis->max) {
    int raw = axis->raw;
    raw = raw<axis->min?axis->min:raw;
    raw = raw>axis->max?axis->max:raw;
    axis->calibrated = raw*(raw<axis->norm?axis->low_scale:axis->high_scale);
  }
}

void joystick_readvalues(struct joystick *joystick) {
  // Bitmasks
  uint8_t status_mask = joystick->num==0?3:12;
  uint8_t button_mask = joystick->num==0?48:192;

  // Beliebigen Wert in Joystick-Port schreiben
  cdi_outb(JOYSTICK_PORT,42);
  // Messung starten
  clock_t start = clock();

  // Status lesen und Bitmasks anwenden
  uint8_t status = cdi_inb(JOYSTICK_PORT);
  uint8_t buttons = (status&button_mask)>>(joystick->num*2+4);
  uint8_t lastst = status&status_mask;

  // Zeit messen, bis die Achsenbits des Joysticks 0 sind
  do {
    status = cdi_inb(JOYSTICK_PORT)&status_mask;
    if (status!=lastst) {
      uint8_t change = (status^lastst)>>(joystick->num*2);
      clock_t raw = clock()-start;
      if (change&1) joystick->axis[0].raw = raw;
      if (change&2) joystick->axis[1].raw = raw;
    }
    lastst = status;
  } while (status!=0);

  // Wert in kalibrierten Wert umrechnen
  joystick_raw2cal(&(joystick->axis[0]));
  joystick_raw2cal(&(joystick->axis[1]));

  // Buttons
  joystick->buttons[0] = buttons&1;
  joystick->buttons[1] = buttons&2;
}

int joystick_read(struct cdi_generic_device *device,void *vdobj,size_t count) {
  size_t i;
  struct joystick *joystick = (struct joystick*)device;
  struct cdi_generic_joystick *obj = (struct cdi_generic_joystick*)vdobj;

  for (i=0;i<count;i++) {
    joystick_readvalues(joystick);
    obj[i].axis_raw[0] = joystick->axis[0].raw;
    obj[i].axis_raw[1] = joystick->axis[1].raw;
    obj[i].axis_cal[0] = joystick->axis[0].calibrated;
    obj[i].axis_cal[1] = joystick->axis[1].calibrated;
    obj[i].buttons[0] = joystick->buttons[0];
    obj[i].buttons[1] = joystick->buttons[1];
  }
  return i;
}

void joystick_driver_destroy(struct cdi_driver *driver) {
  cdi_generic_driver_destroy((struct cdi_generic_driver*)driver);
  cdi_ioports_free(JOYSTICK_PORT,1);
  free(joystick_driver);
}

void joystick_device_init(struct cdi_device *device) {
  struct joystick *joystick = (struct joystick*)device;
  size_t i;
  for (i=0;i<2;i++) {
    joystick[i].num = i;
    memset(joystick[i].axis,0,sizeof(joystick[i].axis));
    memset(joystick[i].buttons,0,sizeof(joystick[i].buttons));
  }
  memset(joystick[i].buttons,0,sizeof(joystick[i].buttons));
}

void joystick_device_remove(struct cdi_device *device) {
  /// @todo Was muss man hier machen?
}

int joystick_driver_init() {
  size_t i;
  joystick_driver = malloc(sizeof(struct cdi_generic_driver));
  joystick_driver->drv.name = "joystick";
  joystick_driver->drv.init_device = joystick_device_init;
  joystick_driver->drv.remove_device = joystick_device_remove;
  joystick_driver->drv.destroy = joystick_driver_destroy;

  /// @todo Ueberpruefen ob wie viele Joysticks verfuegbar sind
  for (i=0;i<2;i++) {
    joysticks[i].dev.dev.type = CDI_GENERIC;
    joysticks[i].dev.dev.name = malloc(4);
    snprintf((char*)joysticks[i].dev.dev.name,4,"js%d",i);
    joysticks[i].dev.dev.driver = (struct cdi_driver*)joystick_driver;
    joysticks[i].dev.stream = malloc(sizeof(struct cdi_generic_stream));
    joysticks[i].dev.stream->objsz = sizeof(struct joystick)-offsetof(struct joystick,axis);
    joysticks[i].dev.stream->read = joystick_read;
    joysticks[i].dev.stream->write = NULL;
    cdi_list_push(joystick_driver->drv.devices,&(joysticks[i].dev));
  }

  return 0;
}

#ifdef CDI_STANDALONE
int main(int argc,char *argv[]) {
  cdi_init(argc,argv);
#else
int init_joystick {
  cdi_init(0,NULL);
#endif

  if (joystick_driver_init()!=0) return -1;

  cdi_ioports_alloc(JOYSTICK_PORT,1);

  cdi_generic_driver_register(joystick_driver);

  cdi_run_drivers();

  return 0;
}
