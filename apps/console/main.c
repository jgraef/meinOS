#include <devfs.h>
#include <unistd.h>
#include <stdio.h>

#include "console.h"

/**
 * Initialize console driver
 */
int main() {
  devfs_dev_t *console;

  init_screen();
  init_keyboard();

  devfs_init();
  console = devfs_createdev("console");
  devfs_onread(console,onread);
  devfs_onwrite(console,onwrite);

  devfs_mainloop();
  return 0;
}
