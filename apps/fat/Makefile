-include ../../Makefile.config

all:
	$(APPS_CC) -o fat *.c -lcdi
	$(APPS_STRIP) fat
	cp fat ../../files/bin

debug:
	$(APPS_CC) -o fat *.c -lcdi
	$(APPS_OBJDUMP) -d -S fat > fat.objdump

clean:
	rm fat
 
