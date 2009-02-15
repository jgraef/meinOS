-include ../Makefile.config

KEYBOARD_LAYOUTS = de \
                   en_us \

all: $(KEYBOARD_LAYOUTS)
	./de > ../files/etc/keyboard_layouts/de
	./en_us > ../files/etc/keyboard_layouts/en_us

clean:
	rm -f $(KEYBOARD_LAYOUTS)