#!/bin/sh
make && \
mkisofs -R -b grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o cdrom.iso files/ && \
./run_vm.sh
