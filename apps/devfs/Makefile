-include ../../Makefile.config

all:
	$(APPS_CC) -o devfs devfs.c -lfuse
	$(APPS_STRIP) devfs
	cp devfs ../../files/bin

debug:
	$(APPS_CC) -o devfs devfs.c -lfuse
	$(APPS_OBJDUMP) -d -S devfs > devfs.objdump

clean:
	rm devfs