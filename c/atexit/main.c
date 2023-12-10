#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void *loop(void *);
static void handler(void);

int
main(void)
{
	pthread_t t;

	atexit(handler);
	pthread_create(&t, NULL, loop, NULL);
	sleep(1);
	return EXIT_SUCCESS;
}

void *
loop(void *_)
{
	(void)_;
	for (;;)
		;
	return NULL;
}

void
handler(void)
{
	puts("hello, world!");
}
