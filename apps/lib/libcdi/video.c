
#include "cdi/video.h"

void cdi_video_driver_init(struct cdi_video_driver* driver) {
  cdi_driver_init((struct cdi_driver*)driver);
}

void cdi_video_driver_destroy(struct cdi_video_driver* driver) {
  cdi_driver_destroy((struct cdi_driver*)driver);
}

void cdi_video_driver_register(struct cdi_video_driver* driver) {
  cdi_driver_register((struct cdi_driver*)driver);
}
