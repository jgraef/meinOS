-include ../../../Makefile.config

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

fuse_daemonize.o: fuse_daemonize.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
fuse_destroy.o: fuse_destroy.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
fuse_fs_new.o: fuse_fs_new.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
fuse_loop.o: fuse_loop.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
fuse_lowlevel.o: fuse_lowlevel.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
fuse_main_real.o: fuse_main_real.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
fuse_mount.o: fuse_mount.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
fuse_new.o: fuse_new.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
fuse_parse_cmdline.o: fuse_parse_cmdline.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
fuse_setup.o: fuse_setup.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
fuse_teardown.o: fuse_teardown.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^
fuse_unmount.o: fuse_unmount.c
	$(LIBS_CC) $(LIBS_CFLAGS) -o $@ $^

clean:
	rm -f $(OBJFILES) ../libfuse.a
