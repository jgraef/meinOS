-include ../Makefile.config

OBJFILES = aaacrt0.o \
           biosint.o \
           cpu.o \
           elf.o \
           gdt.o \
           idt.o \
           interrupt.o \
           ioport.o \
           ipc.o \
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
           dyncall.o \
           llist.o \
           pack.o \
           perm.o \
           tree.o \
           string.o

all: kernel2 kernel2.objdump
	cp $< ../files

kernel2: $(OBJFILES) libgcc.a
	$(KERNEL_LD) -T link.ld -o $@ $(OBJFILES) libgcc.a

aaacrt0.o: crt0.asm
	$(KERNEL_ASM) -f elf -o $@ $^
biosint.o: biosint.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
cpu.o: cpu.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
elf.o: elf.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
gdt.o: gdt.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
idt.o: idt.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
interrupt.o: interrupt.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
ioport.o: ioport.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
ipc.o: ipc.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
isr.0: isr.asm
	$(KERNEL_ASM) -f elf -o $@ $^
kprint.o: kprint.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
lapic.o: lapic.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
main.o: main.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
malloc.o: malloc.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
memkernel.o: memkernel.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
memphys.o: memphys.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
memuser.o: memuser.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
msg.o: msg.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
multiboot.o: multiboot.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
paging.o: paging.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
pic.o: pic.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
procm.o: procm.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
rpc.o: rpc.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
sem.o: sem.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
shm.o: shm.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
signal.o: signal.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
swap.o: swap.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
syscall.o: syscall.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
tss.o: tss.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
vga.o: vga.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
vm86.o: vm86.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
dyncall.o: ../lib/libmeinos/dyncall.asm
	$(KERNEL_ASM) -f elf -o $@ $^
llist.o: ../lib/libmeinos/llist.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
pack.o: ../lib/libmeinos/pack.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
perm.o: ../lib/libmeinos/perm.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
tree.o: ../lib/libmeinos/tree.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^
string.o: ../lib/stdlibc/string.c
	$(KERNEL_CC) $(KERNEL_CFLAGS) -o $@ $^

kernel2.objdump: kernel2
	$(KERNEL_OBJDUMP) -d -S $^ > $@

clean:
	rm -f $(OBJFILES) kernel2 *.objdump
