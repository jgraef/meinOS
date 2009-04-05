-include ../Makefile.config

CC = $(KERNEL_CC)
CFLAGS = $(KERNEL_CFLAGS)

OBJFILES = aaacrt0.o \
           biosint.o \
           cpu.o \
           elf.o \
           gdt.o \
           idt.o \
           interrupt.o \
           ioport.o \
           ipc.o \
           isr.o \
           kprint.o \
           lapic.o \
           main.o \
           malloc.o \
           memkernel.o \
           memphys.o \
           memuser.o \
           msg.o \
           multiboot.o \
           paging.o \
           pic.o \
           procm.o \
           rpc.o \
           sem.o \
           shm.o \
           signal.o \
           swap.o \
           syscall.o \
           tss.o \
           vga.o \
           vm86.o \
           \
           ../lib/libmeinos/dyncall.o \
           ../lib/libmeinos/llist.o \
           ../lib/libmeinos/pack.o \
           ../lib/libmeinos/perm.o \
           ../lib/libmeinos/tree.o \
           ../lib/stdlibc/string.o \
           ../lib/stdlibc/strings.o \
           ../lib/stdlibc/ctype.o

all: kernel2 kernel2.objdump
	cp $< ../files

kernel2: $(OBJFILES) libgcc.a
	$(KERNEL_LD) -T link.ld -o $@ $(OBJFILES) libgcc.a

aaacrt0.o: crt0.asm
	$(KERNEL_ASM) -f elf -o $@ $^
isr.o: isr.asm
	$(KERNEL_ASM) -f elf -o $@ $^

../lib/libmeinos/dyncall.o: ../lib/libmeinos/dyncall.asm
	$(KERNEL_ASM) -f elf -o $@ $^

kernel2.objdump: kernel2
	$(KERNEL_OBJDUMP) -d -S $^ > $@

clean:
	rm -f $(OBJFILES) kernel2 *.objdump
