-include ../../../Makefile.config
all:
	$(LIBS_CC) -c *.c $(LIBS_CFLAGS)
	$(LIBS_AR) rs ../libfuse.a *.o

clean:
	rm *.o
