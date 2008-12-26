-include ../../Makefile.config

all: pci pci.objdump
	cp $< ../../files/bin

pci: pci.c
	$(APPS_CC) -o $@ $^

pci.objdump: pci
	$(APPS_OBJDUMP) -d -S $^ > $@

clean:
	rm -f pci *.objdump
