-include ../../Makefile.config

all: console console.objdump
	cp $< ../../files/bin

console: main.c keyboard.c screen.c
	$(APPS_CC) -o $@ $^

console.objdump: console
	$(APPS_OBJDUMP) -d -S $^ > $@

clean:
	rm -f console *.objdump
