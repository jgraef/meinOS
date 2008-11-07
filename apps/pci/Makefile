-include ../../Makefile.config

all:
	$(APPS_CC) -o pci *.c -lcdi
	$(APPS_STRIP) pci
	cp pci ../../files/bin

debug:
	$(APPS_CC) -o pci *.c -lcdi
	$(APPS_OBJDUMP) -d -S pci > pci.objdump

clean:
	rm pci
