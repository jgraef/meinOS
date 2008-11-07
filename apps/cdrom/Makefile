-include ../../Makefile.config

all:
	$(APPS_CC) -o cdrom *.c
	$(APPS_STRIP) cdrom
	cp cdrom ../../files/bin

debug:
	$(APPS_CC) -o cdrom *.c
	$(APPS_OBJDUMP) -d -S cdrom > cdrom.objdump

clean:
	rm cdrom
