-include ../../Makefile.config

all:
	$(LIBS_ASM) -f elf -o dyncall.o dyncall.asm
	$(LIBS_CC) -c *.c $(LIBS_CFLAGS)

clean:
	rm -f *.o