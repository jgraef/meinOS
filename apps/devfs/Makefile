-include ../../Makefile.config

all: devfs devfs.objdump
	cp $< ../../files/bin

devfs: devfs.c
	$(APPS_CC) -o $@ $^ -lfuse

devfs.objdump: devfs
	$(APPS_OBJDUMP) -d -S $^ > $@

clean:
	rm -f devfs *.objdump