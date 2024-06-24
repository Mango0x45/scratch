#define _GNU_SOURCE
#include <sys/stat.h>

#include <err.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <x86intrin.h>

#include <bsd/stdlib.h>

#define MIN(x, y) ((x) < (y) ? (x) : (y))

static char *readfile(const char *, size_t *);

static bool
skpcmnt(const char **ptr, const char *end)
{
	int nst = 0;
	const char *p = *ptr, needles[] = {'/', '*', 0, 0, 0, 0, 0, 0,
	                                   0,   0,   0, 0, 0, 0, 0, 0};
	const __m128i set = _mm_loadu_si128((const __m128i *)needles);

	while (p < end) {
		ptrdiff_t len = end - p;
		size_t blksz = MIN(len, 16);
		__m128i blk = _mm_loadu_si128((const __m128i *)p);
		int off = _mm_cmpestri(set, 2, blk, blksz, _SIDD_CMP_EQUAL_ANY);

		if (off == 16) {
			p += 16;
			continue;
		}

		if (p[off] == '*' && p[off + 1] == '/') {
			p += off + 2;
			if (--nst == 0) {
				*ptr = p;
				return true;
			}
		} else if (p[off] == '/' && p[off + 1] == '*') {
			p += off + 2;
			nst++;
		} else
			p += off + 1;
	}

	return false;
}

int
main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s file\n", getprogname());
		exit(EXIT_FAILURE);
	}

	size_t len;
	int rv = EXIT_SUCCESS;
	const char *beg = readfile(argv[1], &len);
	const char *end = beg + len;

	clock_t tmbeg = clock();
	while ((beg = memmem(beg, end - beg, "/*", 2)) != NULL) {
		if (!skpcmnt((const char **)&beg, end)) {
			warnx("Unterminated comment!");
			rv = EXIT_FAILURE;
			break;
		}
	}
	clock_t tmend = clock();
	printf("Elapsed time: %.3fs\n", (double)(tmend - tmbeg) / CLOCKS_PER_SEC);

	return rv;
}

char *
readfile(const char *filename, size_t *n)
{
	int fd = open(filename, O_RDONLY);
	if (fd == -1)
		err(1, "open: %s", filename);

	struct stat sb;
	if (fstat(fd, &sb) == -1)
		err(1, "fstat: %s", filename);

	char *p = malloc(sb.st_size + 4);
	if (p == NULL)
		err(1, "malloc");

	ssize_t nr;
	for (size_t off = 0; (nr = read(fd, p + off, sb.st_blksize)) > 0; off += nr)
		;
	if (nr == -1)
		err(1, "read: %s", filename);

	p[sb.st_size + 0] =
	p[sb.st_size + 1] =
	p[sb.st_size + 2] =
	p[sb.st_size + 3] = 0;

	*n = sb.st_size;
	close(fd);
	return p;
}
