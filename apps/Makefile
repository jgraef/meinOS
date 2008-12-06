-include ../Makefile.config

all:
	cp -R include/* $(GCC_TOOLCHAIN)/include/

	make -C lib/
	make -C sh/
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
	make -C console/
	make -C pci/
	make -C psdev/
	make -C login/
	make -C gzip/
	make -C utils/
	make -C cirrus/
	make -C fat/

	#make -C joystick/
	#make -C ext2/

clean:
	make -C lib/ clean
	make -C sh/ clean
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
	make -C console/ clean
	make -C pci/ clean
	make -C psdev/ clean
	make -C login/ clean
	make -C gzip/ clean
	make -C utils/ clean
	make -C cirrus/ clean
	make -C fat/ clean

	#make -C joystick/ clean
	#make -C ext2/ clean
