-include ../../Makefile.config

all: login login.objdump
	cp $< ../../files/bin

login: login.c
	$(APPS_CC) -o $@ $^ #-DLOGIN

login.objdump: login
	$(APPS_OBJDUMP) -d -S $^ > $@

clean:
	rm -f login *.objdump
