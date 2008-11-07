-include ../Makefile.config

all: de en_us
	./de > ../files/etc/keyboard_layouts/de
	./en_us > ../files/etc/keyboard_layouts/en_us


de: de.c
	$(CC) -o $@ $^

en_us: en_us.c
	$(CC) -o $@ $^

clean:
	rm de en_us