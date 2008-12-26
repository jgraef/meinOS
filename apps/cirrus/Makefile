-include ../../Makefile.config

all: cirrus cirrus.objdump
	cp $< ../../files/bin

cirrus: bitmap.c device.c display.c draw.c main.c vesa.c vram.c
	$(APPS_CC) -o $@ $^ -lcdi

cirrus.objdump: cirrus
	$(APPS_OBJDUMP) -d -S $^ > $@

clean:
	rm -f cirrus *.objdump
