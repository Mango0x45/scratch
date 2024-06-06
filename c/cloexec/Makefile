include ../base.mk

all: cloexec
cloexec: cloexec.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f cloexec
