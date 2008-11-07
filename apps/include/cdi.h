#ifndef _CDI_H_
#define _CDI_H_

#include <cdi/lists.h>
/// @todo remove
#include <stdio.h>

#define CDI_STANDALONE

typedef enum {
    CDI_UNKNOWN    = 0,
    CDI_NETWORK    = 1,
    CDI_STORAGE    = 2,
    CDI_GENERIC    = 3,
    CDI_VIDEO      = 4
} cdi_device_type_t;

struct cdi_driver;
struct cdi_device {
  cdi_device_type_t type;
  const char* name;
  struct cdi_driver* driver;
};

struct cdi_driver {
    cdi_device_type_t   type;
    const char*         name;
    cdi_list_t          devices;
    void (*init_device)(struct cdi_device* device);
    void (*remove_device)(struct cdi_device* device);
    void (*destroy)(struct cdi_driver *driver);
};

/// @note meinOS specific
cdi_list_t cdi_drivers;
cdi_list_t cdi_filesystems;

void cdi_init();
void cdi_run_drivers();
void cdi_driver_init(struct cdi_driver* driver);
void cdi_driver_destroy(struct cdi_driver* driver);
void cdi_driver_register(struct cdi_driver* driver);

#endif
