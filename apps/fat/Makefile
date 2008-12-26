-include ../../Makefile.config

SOURCES = bootsector.c \
         cluster.c \
         dir.c \
         file.c \
         init.c \
         main.c \
         res.c \
         resources.c \
         sector.c

all: fat fat.objdump
	cp $< ../../files/bin

fat: $(SOURCES)
	$(APPS_CC) -o $@ $^ -lcdi

fat.objdump: fat
	$(APPS_OBJDUMP) -d -S $^ > $@

clean:
	rm -f fat *.objdump
 
