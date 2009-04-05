-include ../../../Makefile.config

CC = $(LIBS_CC)
CFLAGS = $(LIBS_CFLAGS)

OBJFILES = bios.o \
           cache.o \
           cdi.o \
           dma.o \
           fs.o \
           misc.o \
           net.o \
           scsi.o \
           storage.o \
           video.o

all: ../libcdi.a

../libcdi.a: $(OBJFILES)
	$(LIBS_AR) rs $@ $^

clean:
	rm -f $(OBJFILES) ../libcdi.a
