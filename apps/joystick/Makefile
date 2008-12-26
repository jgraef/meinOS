-include ../../Makefile.config

all: joystick joystick.objdump
	cp $< ../../files/bin

joystick:
	$(APPS_CC) -o $@ $^

joystick.objdump: joystick
	$(APPS_OBJDUMP) -d -S $^ > $@

clean:
	rm -f joystick *.objdump
