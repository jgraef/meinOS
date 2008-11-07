-include ../../Makefile.config

all:
	$(APPS_CC) -o floppy *.c -lcdi
	$(APPS_STRIP) floppy
	cp floppy ../../files/bin

debug:
	$(APPS_CC) -o floppy floppy.c
	$(APPS_OBJDUMP) -d -S floppy > floppy.objdump

clean:
	rm floppy