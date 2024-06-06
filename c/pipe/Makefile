include ../base.mk

all: pipe
pipe: pipe.c
	$(CC) $(CFLAGS) -lpthread -o $@ $<

clean:
	rm -f pipe
