-include ../../Makefile.config

all:
	$(APPS_CC) -o beep beep.c
	$(APPS_STRIP) beep
	cp beep ../../files/bin

debug:
	$(APPS_CC) -o beep beep.c
	$(APPS_OBJDUMP) -d -S beep > beep.objdump

clean:
	rm beep
