-include ../../Makefile.config

all:
	$(APPS_CC) -o console *.c
	$(APPS_STRIP) console
	cp console ../../files/bin

debug:
	$(APPS_CC) -o console *.c
	$(APPS_OBJDUMP) -d -S console > console.objdump

clean:
	rm console
