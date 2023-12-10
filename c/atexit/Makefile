include ../base.mk

all: atexit
atexit: main.c
	$(CC) $(CFLAGS) -lpthread -o $@ $<

clean:
	rm -f atexit
