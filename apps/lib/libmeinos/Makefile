-include ../../../Makefile.config

CC = $(LIBS_CC)
CFLAGS = $(LIBS_CFLAGS)

OBJFILES = cmos.o \
           devfs.o \
           irq_asm.o \
           irq.o \
           lock.o \
           misc.o \
           path.o \
           pci.o \
           proc.o \
           ringbuf.o \
           rpc.o \
           syscall.o \
           \
           ../../../lib/libmeinos/dyncall.o \
           ../../../lib/libmeinos/llist.o \
           ../../../lib/libmeinos/pack.o \
           ../../../lib/libmeinos/perm.o \
           ../../../lib/libmeinos/tree.o

all: ../libmeinos.a

../libmeinos.a: $(OBJFILES)
	$(LIBS_AR) rs $@ $^

irq_asm.o: irq.asm
	$(LIBS_ASM) -f elf -o $@ $^
lock.o: lock.asm
	$(LIBS_ASM) -f elf -o $@ $^

../../../lib/libmeinos/dyncall.o: ../../../lib/libmeinos/dyncall.asm
	$(LIBS_ASM) -f elf -o $@ $^


clean:
	rm -f $(OBJFILES) ../libmeinos.a