include ../base.mk

all: mmap-race
mmap-race: main.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f mmap-race
