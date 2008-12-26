#!/bin/sh
make stacktrace && \
make 1>/dev/null 2>/dev/null && \
mkisofs -R -b grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o cdrom.iso files/ 1>/dev/null 2>/dev/null && \
./run_vm.sh | tee /tmp/meinos.output && \
cat /tmp/meinos.output | ./stacktrace && \
rm -f /tmp/meinos.output && \
make clean 1>/dev/null 2>/dev/null