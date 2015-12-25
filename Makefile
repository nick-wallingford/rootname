CFLAGS += -std=c99 -pipe
LDLIBS += $(shell pkg-config --libs x11)

rootname: rootname.o

clean:
	rm -f rootname Makefile~ rootname.c~
