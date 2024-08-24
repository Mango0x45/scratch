include ../base.mk

all: unicode
unicode: unicode.c unicode.h
	$(CC) $(CFLAGS) -mavx512f -lbsd -o $@ $@.c

clean:
	rm -f unicode
