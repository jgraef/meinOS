-include ../../../Makefile.config

OBJFILES = crypt.o \
           crypt_stub.o \
           des.o \
           md5.o

all: ../libcrypt.a

../libcrypt.a: $(OBJFILES)
	$(LIBS_AR) rs $@ $^

crypt.o: crypt.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
crypt_stub.o: crypt_stub.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
des.o: des.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
md5.o: md5.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^

clean:
	rm -f $(OBJFILES) ../libcrypt.a
