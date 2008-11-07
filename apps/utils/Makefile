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

clean:
	rm false kill mkdir mkfifo mknod readlink true sleep uname echo cat *.o
