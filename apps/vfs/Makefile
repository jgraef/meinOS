-include ../../Makefile.config

all: vfs vfs.objdump
	cp $< ../../files/bin

vfs: vfs.c
	$(APPS_CC) -o $@ $^

vfs.objdump: vfs
	$(APPS_OBJDUMP) -d -S $^ > $@

clean:
	rm -f vfs *.objdump
