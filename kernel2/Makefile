-include ../Makefile.config

all: kernel2 kernel2.objdump
	cp $< ../files

kernel2:
	$(KERNEL_ASM) -f elf -o aaacrt0.o crt0.asm
	$(KERNEL_ASM) -f elf -o isr.o isr.asm
	$(KERNEL_CC) -g -c *.c $(KERNEL_CFLAGS) -Iinclude -I../include
	$(KERNEL_LD) -T link.ld -o $@ *.o $(KERNEL_LIBS)

kernel2.objdump: kernel2
	$(KERNEL_OBJDUMP) -d -S $^ > $@

clean:
	rm -f *.o kernel2 *.objdump
