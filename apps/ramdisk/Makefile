-include ../../Makefile.config

SOURCES = dir.c \
          file.c \
          init.c \
          link.c \
          main.c \
          res.c \
          resources.c

all: ramdisk ramdisk.objdump
	cp $< ../../files/bin

ramdisk: $(SOURCES)
	$(APPS_CC) -o $@ $^ -lcdi

ramdisk.objdump: ramdisk
	$(APPS_OBJDUMP) -d -S $^ > $@

clean:
	rm -f ramdisk *.objdump
