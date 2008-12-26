-include ../../Makefile.config

all: test test.objdump
	cp $< ../../files/bin

test: test.c
	$(APPS_CC) -o $@ $^

test.objdump: test
	$(APPS_OBJDUMP) -d -S $^ > $@

clean:
	rm -f test *.objdump
