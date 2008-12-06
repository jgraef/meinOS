-include ../../Makefile.config

all:
	$(APPS_CC) -o sh *.c
	$(APPS_STRIP) sh
	cp sh ../../files/bin

debug:
	$(APPS_CC) -o sh *.c
	$(APPS_OBJDUMP) -d -S sh > sh.objdump

clean:
	rm sh
