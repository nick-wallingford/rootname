CFLAGS += -std=c99 -pipe $(shell pkg-config --cflags xcb) -Wall -Wextra
LDLIBS += $(shell pkg-config --libs xcb)

rootname: rootname.o

clean:
	rm -f rootname Makefile~ rootname.c~ rootname.o
