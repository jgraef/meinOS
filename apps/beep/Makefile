-include ../../Makefile.config

all: beep

beep: beep.c
	$(APPS_CC) -o $@ $^
	$(APPS_STRIP) $@
	cp $@ ../../files/bin

debug: beep
	$(APPS_OBJDUMP) -d -S $^ > beep.objdump

clean:
	rm -f beep
