-include ../../Makefile.config

all: cdrom cdrom.objdump
	cp $< ../../files/bin

cdrom: main.c device.c
	$(APPS_CC) -o $@ $^

cdrom.objdump: cdrom
	$(APPS_OBJDUMP) -d -S $^ > $@

clean:
	rm -f cdrom *.objdump
