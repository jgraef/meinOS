-include ../../Makefile.config

all: all_rc all_boot

all_rc: rc rc.objdump
	cp $< ../../files/etc/init.d

rc: rc.c helper.c
	$(APPS_CC) -o $@ $^

rc.objdump: rc
	$(APPS_OBJDUMP) -d -S $^ > $@

all_boot: boot boot.objdump
	cp $< ../../files/etc/init.d

boot: boot.c helper.c
	$(APPS_CC) -o $@ $^

boot.objdump: boot
	$(APPS_OBJDUMP) -d -S $^ > $@

clean:
	rm -f rc boot ../../files/etc/init.d/rc ../../files/etc/init.d/boot *.objdump
