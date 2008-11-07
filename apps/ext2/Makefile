-include ../../Makefile.config

all:
	$(APPS_CC) -o ext2 *.c libext2/*.c -Ilibext2/include -lcdi
	$(APPS_STRIP) ext2
	cp ext2 ../../files/bin

debug:
	$(APPS_CC) -o ext2 *.c libext2/*.c -Ilibext2/include -lcdi
	$(APPS_OBJDUMP) -d -S ext2 > ext2.objdump

clean:
	rm ext2
