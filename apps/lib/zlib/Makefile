-include ../../../Makefile.config

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

adler32.o: adler32.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
compress.o: compress.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
crc32.o: crc32.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
deflate.o: deflate.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
gzio.o: gzio.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
infback.o: infback.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
inffast.o: inffast.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
inflate.o: inflate.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
inftrees.o: inftrees.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
trees.o: trees.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
uncompr.o: uncompr.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
zutil.o: zutil.c

clean:
	rm -f $(OBJFILES) ../libz.a
