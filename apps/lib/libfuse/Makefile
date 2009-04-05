-include ../../../Makefile.config

CC = $(LIBS_CC)
CFLAGS = $(LIBS_CFLAGS)

OBJFILES = fuse_daemonize.o \
           fuse_destroy.o \
           fuse_fs_new.o \
           fuse_loop.o \
           fuse_lowlevel.o \
           fuse_main_real.o \
           fuse_mount.o \
           fuse_new.o \
           fuse_parse_cmdline.o \
           fuse_setup.o \
           fuse_teardown.o \
           fuse_unmount.o

all: ../libfuse.a

../libfuse.a: $(OBJFILES)
	$(LIBS_AR) rs $@ $^

clean:
	rm -f $(OBJFILES) ../libfuse.a
