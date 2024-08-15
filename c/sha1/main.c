#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sha1.h"

static uint8_t bigbuf1[1000000];
static uint8_t bigbuf2[1000000];

int
main(int argc, char **argv)
{
	(void)argc;

	int e = 0;
	sha1_t s;
	uint8_t dgst[SHA1DGSTSZ];

	memset(bigbuf1, 'a', sizeof(bigbuf1));
	memset(bigbuf2, 'b', sizeof(bigbuf2));

	sha1init(&s);
	e |= sha1hash(&s, bigbuf1, sizeof(bigbuf1));
	e |= sha1hash(&s, bigbuf2, sizeof(bigbuf2));
	sha1end(&s, dgst);

	if (e != 0) {
		fprintf(stderr, "%s: %s\n", argv[0], strerror(e));
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < sizeof(dgst); i++)
		printf("%02" PRIx8, dgst[i]);
	putchar('\n');
	return EXIT_SUCCESS;
}
