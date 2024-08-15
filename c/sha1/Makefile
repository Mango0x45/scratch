all: sha1-naïve sha1-x86

sha1-naïve: main.c sha1-naïve.c
	cc -flto -O3 -o $@ main.c $@.c

sha1-x86: main.c sha1-x86.c
	cc -flto -O3 -msha -msse4.1 -o $@ main.c $@.c

expected:
	{ yes 'a' | tr -d '\n' | head -c1000000;   \
	  yes 'b' | tr -d '\n' | head -c1000000; } \
	| sha1sum | cut -d' ' -f1

clean:
	rm -f sha1-naïve sha1-x86
