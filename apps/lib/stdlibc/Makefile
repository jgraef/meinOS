-include ../../../Makefile.config

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
           ctype.o \
           string.o \
           strings.o \


all: ../libc.a

../libc.a: $(OBJFILES)
	$(LIBS_AR) rs $@ $^

abort.o: abort.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
asprintf.o: asprintf.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
assert.o: assert.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
env.o: env.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
exec.o: exec.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
files.o: files.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
fork_asm.o: fork.asm
	$(LIBS_ASM) -f elf -o $@ $<
fork.o: fork.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
fprintf.o: fprintf.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
getopt.o: getopt.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
inet.o: inet.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
ipc.o: ipc.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
malloc.o: malloc.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
msg.o: msg.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
pathname.o: pathname.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
printf.o: printf.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
pwd.o: pwd.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
select.o: select.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
sem.o: sem.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
setjmp.o: setjmp.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
shm.o: shm.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
signal_asm.o: signal.asm
	$(LIBS_ASM) -f elf -o $@ $<
signal.o: signal.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
snprintf.o: snprintf.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
sprintf.o: sprintf.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
sscanf.o: sscanf.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
stdio.o: stdio.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
stdlib.o: stdlib.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
sys_time.o: sys_time.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
time.o: time.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
times.o: times.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
uio.o: uio.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
unistd.o: unistd.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
utsname.o: utsname.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
vasprintf.o: vasprintf.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
vfprintf.o: vfprintf.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
vprintf.o: vprintf.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
vsnprintf.o: vsnprintf.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
vsprintf.o: vsprintf.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
vsscanf.o: vsscanf.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
wchar.o: wchar.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
locale.o: locale.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
string.o: ../../../lib/stdlibc/string.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
strings.o: ../../../lib/stdlibc/strings.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)
ctype.o: ../../../lib/stdlibc/ctype.c
	$(LIBS_CC) -o $@ $< $(LIBS_CFLAGS)

clean:
	rm -f $(OBJFILES) ../libc.a
