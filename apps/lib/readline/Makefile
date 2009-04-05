-include ../../../Makefile.config

CC = $(LIBS_CC)
CFLAGS = $(LIBS_CFLAGS)

OBJFILES = readline.o

all: ../libreadline.a

../libreadline.a: $(OBJFILES)
	$(LIBS_AR) rs $@ $^

clean:
	rm -f $(OBJFILES) ../libreadline.a
