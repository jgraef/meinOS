-include ../../Makefile.config

all:
	$(APPS_CC) -o vterm vterm.c
	$(APPS_STRIP) vterm
	cp vterm ../../files/bin

debug:
	$(APPS_CC) -o vterm *.c -lcdi
	$(APPS_OBJDUMP) -d -S vterm > vterm.objdump

clean:
	rm vterm