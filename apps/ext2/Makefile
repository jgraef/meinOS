-include ../../Makefile.config

SOURCES = dir.c \
          file.c \
          init.c \
          libext2_cache.c \
          main.c \
          res.c \
          resources.c \
          symlinks.c \
          libext2/directory.c \
          libext2/file.c \
          libext2/fs.c \
          libext2/inode.c \
          libext2/superblock.c \
          libext2/symlink.c

all: ext2 ext2.objdump
	cp $< ../../files/bin

ext2: $(SOURCES)
	$(APPS_CC) -o $@ $^ -Ilibext2/include -lcdi

ext2.objdump: ext2
	$(APPS_OBJDUMP) -d -S $^ > $@

clean:
	rm -f ext2 *.objdump
