-include ../../Makefile.config

all:
	$(APPS_CC) -o dma dma.c
	$(APPS_STRIP) dma
	cp dma ../../files/bin

debug:
	$(APPS_CC) -o dma dma.c
	$(APPS_OBJDUMP) -d -S dma > dma.objdump

clean:
	rm dma