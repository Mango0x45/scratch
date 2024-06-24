include ../base.mk

all: skpcmnt
skpcmnt: skpcmnt.c
	$(CC) $(CFLAGS) -lbsd -o $@ $<

clean:
	rm -f skpcmnt
