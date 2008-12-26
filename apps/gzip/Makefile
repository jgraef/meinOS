-include ../../Makefile.config

SOURCES = bits.c \
          deflate.c \
          gzip.c \
          inflate.c \
          trees.c \
          unzip.c \
          util.c \
          zip.c

all: gzip gzip.objdump
	cp $< ../../files/bin

gzip: $(SOURCES)
	$(APPS_CC) -o $@ $^ -lz

gzip.objdump: gzip
	$(APPS_OBJDUMP) -d -S $^ > $@

clean:
	rm -f gzip *.objdump
