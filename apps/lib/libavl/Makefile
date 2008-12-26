-include ../../../Makefile.config

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

avl.o: avl.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
bst.o: bst.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
pavl.o: pavl.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
pbst.o: pbst.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
prb.o: prb.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
rb.o: rb.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
rtavl.o: rtavl.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
rtbst.o: rtbst.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
rtrb.o: rtrb.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
tavl.o: tavl.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
tbst.o: tbst.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
trb.o: trb.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^

clean:
	rm -f $(OBJFILES) ../libavl.a
