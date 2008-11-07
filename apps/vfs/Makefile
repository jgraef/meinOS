-include ../../Makefile.config

all:
	$(APPS_CC) -o vfs vfs.c
	$(APPS_STRIP) vfs
	cp vfs ../../files/bin

debug:
	$(APPS_CC) -o vfs *.c -lcdi
	$(APPS_OBJDUMP) -d -S vfs > vfs.objdump

clean:
	rm vfs