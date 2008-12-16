-include ../../Makefile.config

all:
	$(APPS_CC) -c -o file_mode.o file_mode.c

	$(APPS_CC) -o false false.c
	$(APPS_STRIP) false
	cp false ../../files/bin

	$(APPS_CC) -o kill kill.c
	$(APPS_STRIP) kill
	cp kill ../../files/bin

	$(APPS_CC) -o mkdir mkdir.c file_mode.o
	$(APPS_STRIP) mkdir
	cp mkdir ../../files/bin

	$(APPS_CC) -o mkfifo mkfifo.c file_mode.o
	$(APPS_STRIP) mkfifo
	cp mkfifo ../../files/bin

	$(APPS_CC) -o mknod mknod.c file_mode.o
	$(APPS_STRIP) mknod
	cp mknod ../../files/bin

	$(APPS_CC) -o readlink readlink.c
	$(APPS_STRIP) readlink
	cp readlink ../../files/bin

	$(APPS_CC) -o true true.c
	$(APPS_STRIP) true
	cp true ../../files/bin

	$(APPS_CC) -o sleep sleep.c
	$(APPS_STRIP) sleep
	cp sleep ../../files/bin

	$(APPS_CC) -o uname uname.c
	$(APPS_STRIP) uname
	cp uname ../../files/bin

	$(APPS_CC) -o echo echo.c
	$(APPS_STRIP) echo
	cp echo ../../files/bin

	$(APPS_CC) -o cat cat.c
	$(APPS_STRIP) cat
	cp cat ../../files/bin

	$(APPS_CC) -o ln ln.c
	$(APPS_STRIP) ln
	cp ln ../../files/bin

	$(APPS_CC) -o id id.c
	$(APPS_STRIP) id
	cp id ../../files/bin

	$(APPS_CC) -o whoami whoami.c
	$(APPS_STRIP) whoami
	cp whoami ../../files/bin

	$(APPS_CC) -o yes yes.c
	$(APPS_STRIP) yes
	cp yes ../../files/bin

	$(APPS_CC) -o basename basename.c
	$(APPS_STRIP) basename
	cp basename ../../files/bin

	$(APPS_CC) -o chmod chmod.c
	$(APPS_STRIP) chmod
	cp chmod ../../files/bin

	$(APPS_CC) -o chown chown.c
	$(APPS_STRIP) chown
	cp chown ../../files/bin

	$(APPS_CC) -o factor factor.c
	$(APPS_STRIP) factor
	cp factor ../../files/bin

	$(APPS_CC) -o pwd pwd.c
	$(APPS_STRIP) pwd
	cp pwd ../../files/bin

clean:
	rm -f false kill mkdir mkfifo mknod readlink true sleep uname echo cat ln id whoami yes basename chmod chown factor pwd *.o
