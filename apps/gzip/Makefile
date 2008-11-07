-include ../../Makefile.config

all:
	$(APPS_CC) -o gzip *.c -lz
	$(APPS_STRIP) gzip
	cp gzip ../../files/bin

debug:
	$(APPS_CC) -o gzip *.c -lz
	$(APPS_OBJDUMP) -d -S gzip > gzip.objdump

clean:
	rm gzip
