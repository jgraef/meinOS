-include ../../Makefile.config

all:
	$(APPS_CC) -o mount mount.c
	$(APPS_STRIP) mount
	cp mount ../../files/bin

debug:
	$(APPS_CC) -o mount *.c
	$(APPS_OBJDUMP) -d -S mount > mount.objdump

clean:
	rm -f mount
