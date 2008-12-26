-include ../../Makefile.config

all: libc.a libmeinos.a libcdi.a libfuse.a libz.a libreadline.a libavl.a libcrypt.a crt0.o
	cp $^ $(GCC_TOOLCHAIN)/lib/
	# HACK!
	# merge libmeinos and libc together
	ar qs $(GCC_TOOLCHAIN)/lib/libc.a ../../lib/libmeinos/*.o
	ar qs $(GCC_TOOLCHAIN)/lib/libc.a libmeinos/*.o

libc.a:
	make -C stdlibc/
	ar qs libc.a ../../lib/stdlibc/*.o

libmeinos.a:
	make -C libmeinos/
	ar qs libmeinos.a ../../lib/libmeinos/*.o

libcdi.a:
	make -C libcdi/

libfuse.a:
	make -C libfuse/

libz.a:
	make -C zlib/

libreadline.a:
	make -C readline/

libavl.a:
	make -C libavl/

libcrypt.a:
	make -C libcrypt/

crt0.o:
	make -C crt0/

clean:
	make -C libmeinos/ clean
	make -C stdlibc/ clean
	make -C libcdi/ clean
	make -C libfuse/ clean
	make -C zlib/ clean
	make -C readline/ clean
	make -C libavl/ clean
	make -C libcrypt/ clean
	make -C crt0/ clean

	rm -f *.a *.o