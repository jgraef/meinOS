-include ../../../Makefile.config

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

bios.o: bios.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
cache.o: cache.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
cdi.o: cdi.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
dma.o: dma.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
fs.o: fs.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
misc.o: misc.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
net.o: net.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
pci.o: pci.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
scsi.o: scsi.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
storage.o: storage.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
video.o: video.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^

clean:
	rm -f $(OBJFILES) ../libcdi.a
