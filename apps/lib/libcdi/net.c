#include <sys/types.h>
#include <string.h>
#include <cdi.h>
#include <cdi/net.h>

void cdi_net_driver_init(struct cdi_net_driver* driver) {
  cdi_driver_init((struct cdi_driver*)driver);
}

void cdi_net_driver_destroy(struct cdi_net_driver* driver) {
  cdi_driver_destroy((struct cdi_driver*)driver);
  memset(&driver,0,sizeof(driver));
}

void cdi_net_driver_register(struct cdi_net_driver* driver) {
  cdi_driver_register((struct cdi_driver*)driver);
}

void cdi_net_device_init(struct cdi_net_device* device) {

}

void cdi_net_receive(struct cdi_net_device* device,void *buffer,size_t size) {

}
