-include ../../Makefile.config

all:
	$(APPS_CC) -o ata *.c -lcdi
	$(APPS_STRIP) ata
	cp ata ../../files/bin

debug:
	$(APPS_CC) -o ata *.c -lcdi
	$(APPS_OBJDUMP) -d -S ata > ata.objdump

clean:
	rm ata
