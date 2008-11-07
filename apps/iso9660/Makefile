-include ../../Makefile.config

all:
	$(APPS_CC) -o iso9660 *.c -lcdi
	$(APPS_STRIP) iso9660
	cp iso9660 ../../files/bin

debug:
	$(APPS_CC) -o iso9660 *.c -lcdi
	$(APPS_OBJDUMP) -d -S iso9660 > iso9660.objdump

clean:
	rm iso9660
