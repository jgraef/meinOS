-include ../../Makefile.config

all:
	$(APPS_CC) -o login login.c #-DLOGIN
	$(APPS_STRIP) login
	cp login ../../files/bin

debug:
	$(APPS_CC) -o login login.c #-DLOGIN
	$(APPS_OBJDUMP) -d -S login > login.objdump

clean:
	rm login
