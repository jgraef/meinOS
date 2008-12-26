-include ../../Makefile.config

all: ata ata.objdump
	cp $< ../../files/bin

ata: ata.c atapi.c device.c libpartition.c main.c request.c
	$(APPS_CC) -o $@ $^ -lcdi

ata.objdump: ata
	$(APPS_OBJDUMP) -d -S $^ > $@

clean:
	rm -f ata *.objdump
