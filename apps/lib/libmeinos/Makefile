-include ../../../Makefile.config
all:
	$(LIBS_ASM) -f elf -o irq_asm.o irq.asm
	#$(LIBS_ASM) -f elf -o lock.o lock.asm
	$(LIBS_CC) -c *.c $(LIBS_CFLAGS)
	$(LIBS_AR) rs ../libmeinos.a *.o ../../../lib/libmeinos/*.o
clean:
	rm *.o