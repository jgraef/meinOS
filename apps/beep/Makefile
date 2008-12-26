-include ../../Makefile.config

all: beep beep.objdump
	cp $< ../../files/bin

beep: beep.c
	$(APPS_CC) -o $@ $^

beep.objdump: beep
	$(APPS_OBJDUMP) -d -S $^ > $@

clean:
	rm -f beep *.objdump
