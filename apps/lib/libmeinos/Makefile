-include ../../../Makefile.config

OBJFILES = cmos.o \
           devfs.o \
           execute.o \
           irq_asm.o \
           irq.o \
           llist2.o \
           lock.o \
           misc.o \
           path.o \
           pci.o \
           proc.o \
           ringbuf.o \
           rpc.o \
           syscall.o \
           dyncall.o \
           llist.o \
           pack.o \
           perm.o \
           tree.o

all: ../libmeinos.a

../libmeinos.a: $(OBJFILES)
	$(LIBS_AR) rs $@ $^

cmos.o: cmos.c
	$(LIBS_CC) -o $@ $^ $(LIBS_CFLAGS)
devfs.o: devfs.c
	$(LIBS_CC) -o $@ $^ $(LIBS_CFLAGS)
execute.o: execute.c
	$(LIBS_CC) -o $@ $^ $(LIBS_CFLAGS)
irq_asm.o: irq.asm
	$(LIBS_ASM) -f elf -o $@ $^
irq.o: irq.c
	$(LIBS_CC) -o $@ $^ $(LIBS_CFLAGS)
llist2.o: llist2.c
	$(LIBS_CC) -o $@ $^ $(LIBS_CFLAGS)
lock.o: lock.asm
	$(LIBS_ASM) -f elf -o $@ $^
misc.o: misc.c
	$(LIBS_CC) -o $@ $^ $(LIBS_CFLAGS)
path.o: path.c
	$(LIBS_CC) -o $@ $^ $(LIBS_CFLAGS)
pci.o: pci.c
	$(LIBS_CC) -o $@ $^ $(LIBS_CFLAGS)
proc.o: proc.c
	$(LIBS_CC) -o $@ $^ $(LIBS_CFLAGS)
ringbuf.o: ringbuf.c
	$(LIBS_CC) -o $@ $^ $(LIBS_CFLAGS)
rpc.o: rpc.c
	$(LIBS_CC) -o $@ $^ $(LIBS_CFLAGS)
syscall.o: syscall.c
	$(LIBS_CC) -o $@ $^ $(LIBS_CFLAGS)
dyncall.o: ../../../lib/libmeinos/dyncall.asm
	$(LIBS_ASM) -f elf -o $@ $^
llist.o: ../../../lib/libmeinos/llist.c
	$(LIBS_CC) -o $@ $^ $(LIBS_CFLAGS)
pack.o: ../../../lib/libmeinos/pack.c
	$(LIBS_CC) -o $@ $^ $(LIBS_CFLAGS)
perm.o: ../../../lib/libmeinos/perm.c
	$(LIBS_CC) -o $@ $^ $(LIBS_CFLAGS)
tree.o: ../../../lib/libmeinos/tree.c
	$(LIBS_CC) -o $@ $^ $(LIBS_CFLAGS)

clean:
	rm -f $(OBJFILES) ../libmeinos.a