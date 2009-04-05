-include ../../../Makefile.config

CC = $(LIBS_CC)
CFLAGS = $(LIBS_CFLAGS)

OBJFILES = adler32.o \
           compress.o \
           crc32.o \
           deflate.o \
           gzio.o \
           infback.o \
           inffast.o \
           inflate.o \
           inftrees.o \
           trees.o \
           uncompr.o \
           zutil.o

all: ../libz.a

../libz.a: $(OBJFILES)
	$(LIBS_AR) rs $@ $^

clean:
	rm -f $(OBJFILES) ../libz.a
