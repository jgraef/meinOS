-include ../../Makefile.config

all:
	$(APPS_CC) -o psdev *.c -lcdi
	$(APPS_STRIP) psdev
	cp psdev ../../files/bin

debug:
	$(APPS_CC) -o psdev *.c -lcdi
	$(APPS_OBJDUMP) -d -S psdev > psdev.objdump

clean:
	rm psdev
