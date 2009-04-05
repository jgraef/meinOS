-include ../Makefile.config

all: copy_include all_libs all_apps

clean: clean_apps clean_libs


all_libs:
	make -C lib/

all_apps: clean_apps # HACK!
	make -C vterm/
	make -C beep/
	make -C test/
	make -C com/
	make -C devfs/
	make -C floppy/
	make -C vfs/
	make -C init/
	make -C dma/
	make -C ramdisk/
	make -C ata/
	make -C cdrom/
	make -C mount/
	make -C iso9660/
	make -C pci/
	make -C psdev/
	make -C login/
	make -C gzip/
	make -C utils/
	make -C cirrus/
	make -C fat/
	make -C rc/

	#make -C joystick/
	#make -C ext2/

clean_libs:
	make -C lib/ clean

clean_apps:
	### TODO: FIXME #####
	make -C vterm/ clean
	make -C beep/ clean
	make -C test/ clean
	make -C com/ clean
	make -C devfs/ clean
	make -C floppy/ clean
	make -C vfs/ clean
	make -C init/ clean
	make -C dma/ clean
	make -C ramdisk clean
	make -C ata/ clean
	make -C cdrom/ clean
	make -C mount/ clean
	make -C iso9660/ clean
	make -C pci/ clean
	make -C psdev/ clean
	make -C login/ clean
	make -C gzip/ clean
	make -C utils/ clean
	make -C cirrus/ clean
	make -C fat/ clean
	make -C rc/ clean

	#make -C joystick/ clean
	#make -C ext2/ clean

	#####################

copy_include:
	cp -R include/* $(GCC_TOOLCHAIN)/include/
