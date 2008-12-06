-include ../../Makefile.config

all:
	$(APPS_CC) -o init init.c exe_*.c
	$(APPS_CC) -o init2 init2.c
	$(APPS_STRIP) init init2
	cp init init2 ../../files/bin

debug:
	$(APPS_CC) -o init init.c exe_*.c
	$(APPS_CC) -o init2 init2.c
	$(APPS_OBJDUMP) -d -S init > init.objdump
	$(APPS_OBJDUMP) -d -S init2 > init2.objdump

clean:
	rm -f init init2
