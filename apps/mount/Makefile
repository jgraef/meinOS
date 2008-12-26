-include ../../Makefile.config

all: all_mount all_umount

all_mount: mount mount.objdump
	cp $< ../../files/bin

all_umount: umount umount.objdump
	cp $< ../../files/bin

mount: mount.c
	$(APPS_CC) -o $@ $^

umount: umount.c
	$(APPS_CC) -o $@ $^

mount.objdump: mount
	$(APPS_OBJDUMP) -d -S $^ > $@

umount.objdump: umount
	$(APPS_OBJDUMP) -d -S $^ > $@

clean:
	rm -f mount umount *.objdump
