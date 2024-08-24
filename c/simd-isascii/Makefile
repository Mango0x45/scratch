include ../base.mk

all: isascii
isascii: isascii.c
	$(CC) $(CFLAGS) -lbsd -o $@ $<

clean:
	rm -f isascii
