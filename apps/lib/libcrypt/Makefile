-include ../../../Makefile.config

CC = $(LIBS_CC)
CFLAGS = $(LIBS_CFLAGS)

OBJFILES = crypt.o \
           crypt_stub.o \
           des.o \
           md5.o

all: ../libcrypt.a

../libcrypt.a: $(OBJFILES)
	$(LIBS_AR) rs $@ $^

clean:
	rm -f $(OBJFILES) ../libcrypt.a
