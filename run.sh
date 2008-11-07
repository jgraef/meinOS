#!/bin/sh
make
mkisofs -R -b grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o cdrom.iso files/
qemu -cdrom cdrom.iso -m 32 -boot d -serial stdio
make clean