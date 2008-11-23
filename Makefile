-include Makefile.config

CDROM = cdrom.iso

all:
	./test_meinos_xgcc.sh
	make -C keyboard_layouts/
	make -C lib/
	cp -R include/* $(GCC_TOOLCHAIN)/include/
	make -C kernel2/
	make -C apps/

clean:
	make -C keyboard_layouts/ clean
	make -C kernel2/ clean
	make -C lib/ clean
	make -C apps clean
