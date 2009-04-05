-include ../../../Makefile.config

CC = $(LIBS_CC)
CFLAGS = $(LIBS_CFLAGS)

OBJFILES = abort.o \
           asprintf.o \
           assert.o \
           env.o \
           exec.o \
           files.o \
           fork_asm.o \
           fork.o \
           fprintf.o \
           getopt.o \
           inet.o \
           ipc.o \
           malloc.o \
           msg.o \
           pathname.o \
           printf.o \
           pwd.o \
           select.o \
           sem.o \
           setjmp.o \
           shm.o \
           signal_asm.o \
           signal.o \
           snprintf.o \
           sprintf.o \
           sscanf.o \
           stdio.o \
           stdlib.o \
           sys_time.o \
           time.o \
           times.o \
           uio.o \
           unistd.o \
           utsname.o \
           vasprintf.o \
           vfprintf.o \
           vprintf.o \
           vsnprintf.o \
           vsprintf.o \
           vsscanf.o \
           wchar.o \
           wstring.o \
           locale.o \
           \
           regex/regcomp.o \
           regex/regerror.o \
           regex/regexec.o \
           regex/regfree.o \
           \
           ../../../lib/stdlibc/ctype.o \
           ../../../lib/stdlibc/string.o \
           ../../../lib/stdlibc/strings.o

all: ../libc.a

../libc.a: $(OBJFILES)
	$(LIBS_AR) rs $@ $^

fork_asm.o: fork.asm
	$(LIBS_ASM) -f elf -o $@ $<
signal_asm.o: signal.asm
	$(LIBS_ASM) -f elf -o $@ $<

clean:
	rm -f $(OBJFILES) ../libc.a
