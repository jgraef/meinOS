-include ../../../Makefile.config

all:
	$(LIBS_CC) -c *.c $(LIBS_CFLAGS)
	$(LIBS_AR) rs ../libcrypt.a *.o

clean:
	rm -f *.o
