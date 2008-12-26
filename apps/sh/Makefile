-include ../../Makefile.config

all: sh sh.objdump
	cp $< ../../files/bin

sh: main.c
	$(APPS_CC) -o $@ $^ -lreadline

sh.objdump: sh
	$(APPS_OBJDUMP) -d -S $^ > $@

clean:
	rm -f sh *.objdump
