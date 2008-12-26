-include ../../../Makefile.config

OBJFILES = readline.o

all: ../libreadline.a

../libreadline.a: $(OBJFILES)
	$(LIBS_AR) rs $@ $^

readline.o: readline.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^

clean:
	rm -f $(OBJFILES) ../libreadline.a
