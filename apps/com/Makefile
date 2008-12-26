-include ../../Makefile.config

all: com com.objdump
	cp $< ../../files/bin

com: main.c com.c
	$(APPS_CC) -o $@ $^

com.objdump: com
	$(APPS_OBJDUMP) -d -S $^ > $@

clean:
	rm -f com *.objdump
