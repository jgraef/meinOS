-include ../../Makefile.config

all:
	make -C libmeinos
	make -C stdlibc
	make -C libcdi
	make -C libfuse
	make -C zlib
	make -C crt0

	cp libc.a libcdi.a libfuse.a libz.a crt0.o $(GCC_TOOLCHAIN)/lib/
	ar qs $(GCC_TOOLCHAIN)/lib/libc.a ../../lib/libmeinos/*.o
	ar qs $(GCC_TOOLCHAIN)/lib/libc.a libmeinos/*.o

clean:
	make -C libmeinos clean
	make -C stdlibc clean
	make -C libcdi clean
	make -C libfuse clean
	make -C zlib clean
	make -C crt0 clean
	rm libc.a libmeinos.a libcdi.a libfuse.a libz.a crt0.o