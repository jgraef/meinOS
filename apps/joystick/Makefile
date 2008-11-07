-include ../../Makefile.config

all:
	$(APPS_CC) -o joystick joystick.c
	$(APPS_STRIP) joystick
	cp joystick ../../files/bin

debug:
	$(APPS_CC) -o joystick joystick.c
	$(APPS_OBJDUMP) -d -S joystick > joystick.objdump

clean:
	rm joystick
