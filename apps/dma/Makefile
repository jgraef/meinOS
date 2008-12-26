-include ../../Makefile.config

all: dma dma.objdump
	cp $< ../../files/bin

dma: dma.c
	$(APPS_CC) -o $@ $^

dma.objdump: dma
	$(APPS_OBJDUMP) -d -S $^ > $@

clean:
	rm -f dma *.objdump