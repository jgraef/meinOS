-include ../../Makefile.config

all:
	$(APPS_CC) -o com *.c
	$(APPS_STRIP) com
	cp com ../../files/bin

debug:
	$(APPS_CC) -o com *.c
	$(APPS_OBJDUMP) -d -S com > com.objdump

clean:
	rm com
