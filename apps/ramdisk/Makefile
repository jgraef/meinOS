-include ../../Makefile.config

all:
	$(APPS_CC) -o ramdisk *.c -lcdi
	$(APPS_STRIP) ramdisk
	cp ramdisk ../../files/bin

debug:
	$(APPS_CC) -o ramdisk *.c -lcdi
	$(APPS_OBJDUMP) -d -S ramdisk > ramdisk.objdump

clean:
	rm ramdisk
