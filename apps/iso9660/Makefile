-include ../../Makefile.config

SOURCES = dir.c \
          file.c \
          init.c \
          main.c \
          res.c \
          resources.c \
          rockridge.c \
          sector.c \
          volume_descriptor.c

all: iso9660 iso9660.objdump
	cp $< ../../files/bin

iso9660: $(SOURCES)
	$(APPS_CC) -o $@ $^ -lcdi

iso9660.objdump: iso9660
	$(APPS_OBJDUMP) -d -S $^ > $@

clean:
	rm -f iso9660 *.objdump
