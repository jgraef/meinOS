-include ../../../Makefile.config
all:
	$(LIBS_ASM) -f elf -o signal_asm.o signal.asm
	$(LIBS_ASM) -f elf -o fork_asm.o fork.asm
	$(LIBS_CC) -c *.c $(LIBS_CFLAGS)
	$(LIBS_AR) rs ../libc.a *.o

clean:
	rm -f *.o
