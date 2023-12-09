include ../base.mk

all: sigfd
sigfd: main.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f sigfd
