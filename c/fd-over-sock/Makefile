include ../base.mk

all: client server
client: client.c
	$(CC) $(CFLAGS) -o $@ $<
server: server.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f client server
