-include ../../../Makefile.config

all: ../crt0.o

../crt0.o: crt0.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^

clean:
	rm -f ../crt0.o
