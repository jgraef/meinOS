-include ../../Makefile.config

UTILS = false \
        kill \
        mkdir \
        mkfifo \
        mknod \
        readlink \
        true \
        sleep \
        uname \
        echo \
        cat \
        ln \
        id \
        whoami \
        yes \
        basename \
        chown \
        chmod \
        factor \
        pwd \
        dirname \
        tee \
        ipcs \
        asa \
        cmp

all: $(UTILS)
	cp $(UTILS) ../../files/bin

file_mode.o: file_mode.c
	$(APPS_CC) -c -o $@ $^

factor_wheel.h: factor_wheelgen.pl
	./$< 5 > $@

false: false.c
	$(APPS_CC) -o $@ $^

kill: kill.c
	$(APPS_CC) -o $@ $^

mkdir: mkdir.c file_mode.o
	$(APPS_CC) -o $@ $^

mkfifo: mkfifo.c file_mode.o
	$(APPS_CC) -o $@ $^

mknod: mknod.c file_mode.o
	$(APPS_CC) -o $@ $^

readlink: readlink.c
	$(APPS_CC) -o $@ $^

true: true.c
	$(APPS_CC) -o $@ $^

sleep: sleep.c
	$(APPS_CC) -o $@ $^

uname: uname.c
	$(APPS_CC) -o $@ $^

echo: echo.c
	$(APPS_CC) -o $@ $^

cat: cat.c
	$(APPS_CC) -o $@ $^

ln: ln.c
	$(APPS_CC) -o $@ $^

id: id.c
	$(APPS_CC) -o $@ $^

whoami: whoami.c
	$(APPS_CC) -o $@ $^

yes: yes.c
	$(APPS_CC) -o $@ $^

basename: basename.c
	$(APPS_CC) -o $@ $^

chmod: chmod.c
	$(APPS_CC) -o $@ $^

chown: chown.c
	$(APPS_CC) -o $@ $^

factor: factor.c factor_wheel.h
	$(APPS_CC) -o $@ $^

pwd: pwd.c
	$(APPS_CC) -o $@ $^

dirname: dirname.c
	$(APPS_CC) -o $@ $^

tee: tee.c
	$(APPS_CC) -o $@ $^

ipcs: ipcs.c
	$(APPS_CC) -o $@ $^

asa: asa.c
	$(APPS_CC) -o $@ $^

cmp: cmp.c
	$(APPS_CC) -o $@ $^

clean:
	rm -f  $(UTILS) file_mode.o factor_wheel.h
