#include <devfs.h>

#include "driver.h"

int main(int argc,char *argv[]) {
  devfs_init();

  null_init();
  random_init();
  time_init();
  zero_init();

  devfs_mainloop();

  return 0;
}
