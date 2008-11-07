-include ../../Makefile.config

all:
	$(APPS_CC) -o cirrus *.c -lcdi
	$(APPS_STRIP) cirrus
	cp cirrus ../../files/bin

debug:
	$(APPS_CC) -o cirrus *.c -lcdi
	$(APPS_OBJDUMP) -d -S cirrus > cirrus.objdump

clean:
	rm cirrus
