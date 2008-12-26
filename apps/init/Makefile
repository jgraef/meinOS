-include ../../Makefile.config

all: all_init all_init2

all_init: init init.objdump
	cp $< ../../files/bin

all_init2: init2 init2.objdump
	cp $< ../../files/bin

init: init.c exe_elf.c
	$(APPS_CC) -o $@ $^

init2: init2.c
	$(APPS_CC) -o $@ $^

init.objdump: init
	$(APPS_OBJDUMP) -d -S $^ > $@

init2.objdump: init2
	$(APPS_OBJDUMP) -d -S $^ > $@

clean:
	rm -f init init2 *.objdump
