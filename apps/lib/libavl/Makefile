-include ../../../Makefile.config

CC = $(LIBS_CC)
CFLAGS = $(LIBS_CFLAGS)

OBJFILES = avl.o \
           bst.o \
           pavl.o \
           pbst.o \
           prb.o \
           rb.o \
           rtavl.o \
           rtbst.o \
           rtrb.o \
           tavl.o \
           tbst.o \
           trb.o

all: ../libavl.a

../libavl.a: $(OBJFILES)
	$(LIBS_AR) rs $@ $^

clean:
	rm -f $(OBJFILES) ../libavl.a
