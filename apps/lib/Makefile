-include ../../Makefile.config

all:
	make -C stdlibc/
	make -C libm/
	make -C libmeinos/
	make -C libcdi/
	make -C libfuse/
	make -C zlib/
	make -C readline/
	make -C libavl/
	make -C libcrypt/
	make -C crt0/

	cp *.a *.o $(GCC_TOOLCHAIN)/lib/
	# HACK!
	# merge libmeinos and libc together
	ar qs $(GCC_TOOLCHAIN)/lib/libc.a libmeinos/*.o

clean:
	make -C stdlibc/ clean
	make -C libm/ clean
	make -C libmeinos/ clean
	make -C libcdi/ clean
	make -C libfuse/ clean
	make -C zlib/ clean
	make -C readline/ clean
	make -C libavl/ clean
	make -C libcrypt/ clean
	make -C crt0/ clean
