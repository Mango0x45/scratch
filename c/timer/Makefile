include ../base.mk

CFLAGS := $(CFLAGS) $(shell pkg-config --cflags libnotify) -D_ISOC99_SOURCE
LDLIBS = $(shell pkg-config --libs libnotify)

all: timer
timer: main.c
	$(CC) $(CFLAGS) $(LDLIBS) -o $@ $<

clean:
	rm -f timer
