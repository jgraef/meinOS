-include ../../Makefile.config

all: psdev psdev.objdump
	cp $< ../../files/bin

psdev: main.c null.c random.c time.c zero.c
	$(APPS_CC) -o $@ $^

psdev.objdump: psdev
	$(APPS_OBJDUMP) -d -S $^ > $@

clean:
	rm -f psdev *.objdump
