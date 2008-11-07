-include ../../../Makefile.config
all:
	$(LIBS_ASM) -f elf -o signal_asm.o signal.asm
	$(LIBS_CC) -c *.c $(LIBS_CFLAGS)
	$(LIBS_AR) rs ../libc.a *.o ../../../lib/stdlibc/*.o

clean:
	rm *.o
