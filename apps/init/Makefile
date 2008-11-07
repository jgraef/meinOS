-include ../../Makefile.config

all:
	$(APPS_CC) -o init init.c
	$(APPS_STRIP) init
	cp init ../../files/bin

debug:
	$(APPS_CC) -o init *.c
	$(APPS_OBJDUMP) -d -S init > init.objdump

clean:
	rm init
