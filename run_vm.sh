#!/bin/sh

qemu -cdrom cdrom.iso -m 32 -boot d -serial stdio
#bochs -f bochs.cfg -q
