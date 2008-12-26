-include ../../Makefile.config

SOURCES = display.c \
          keyboard.c \
          main.c \
          vterm.c

all: vterm vterm.objdump
	cp $< ../../files/bin

vterm: $(SOURCES)
	$(APPS_CC) -o $@ $^

vterm.objdump: vterm
	$(APPS_OBJDUMP) -d -S $^ > $@

clean:
	rm -f vterm *.objdump
