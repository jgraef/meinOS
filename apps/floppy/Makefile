-include ../../Makefile.config

all: floppy floppy.objdump
	cp $< ../../files/bin

floppy: device.c main.c
	$(APPS_CC) -o $@ $^ -lcdi

floppy.objdump: floppy
	$(APPS_OBJDUMP) -d -S $^ > $@

clean:
	rm -f floppy *.objdump