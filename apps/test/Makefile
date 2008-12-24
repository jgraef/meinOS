-include ../../Makefile.config

all:
	$(APPS_CC) -o test test.c
	$(APPS_STRIP) test
	cp test ../../files/bin

debug:
	$(APPS_CC) -o test test.c
	$(APPS_OBJDUMP) -d -S test > test.objdump

clean:
	rm test
