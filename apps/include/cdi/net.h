#ifndef _CDI_NET_H_
#define _CDI_NET_H_

#include <sys/types.h>
#include <cdi.h>
#include <stdint.h>

struct cdi_net_device {
  struct cdi_device dev;
  uint64_t mac:48;
  void (*send_packet)(struct cdi_net_device* device,void* data,size_t size);
};

struct cdi_net_driver {
  struct cdi_driver drv;
};

void cdi_net_driver_init(struct cdi_net_driver* driver);
void cdi_net_driver_destroy(struct cdi_net_driver* driver);
void cdi_net_device_init(struct cdi_net_device* device);
void cdi_net_receive(struct cdi_net_device* device,void* buffer,size_t size);

#endif
