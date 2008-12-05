-include ../Makefile.config

all:
	$(KERNEL_ASM) -f elf -o aaacrt0.o crt0.asm
	$(KERNEL_ASM) -f elf -o isr.o isr.asm
	$(KERNEL_CC) -c *.c $(KERNEL_CFLAGS) -Iinclude -I../include
	$(KERNEL_LD) -T link.ld -o kernel2 *.o $(KERNEL_LIBS)
	$(KERNEL_STRIP) kernel2
	cp kernel2 ../files

debug:
	$(KERNEL_ASM) -f elf -o aaacrt0.o crt0.asm
	$(KERNEL_ASM) -f elf -o isr.o isr.asm
	$(KERNEL_CC) -g -c *.c $(KERNEL_CFLAGS) -Iinclude -I../include
	$(KERNEL_LD) -T link.ld -o kernel2 *.o *.a ../lib/libmeinos/*.o ../lib/stdlibc/*.o
	$(KERNEL_OBJDUMP) -d -S kernel2 > kernel2.objdump

clean:
	rm *.o kernel2
