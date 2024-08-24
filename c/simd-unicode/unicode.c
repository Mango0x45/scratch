#if !__AVX512F__
#	error "AVX512 intrinsics are required"
#endif

#define _GNU_SOURCE
#include <sys/stat.h>

#include <err.h>
#include <fcntl.h>
#include <immintrin.h>
#include <stdalign.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <bsd/stdlib.h>

#include "unicode.h"

#define MIN(x, y) ((x) < (y) ? (x) : (y))

static int unicode_generic(const char *, size_t);
static int unicode_simd(const char *, size_t);
static char *readfile(const char *, size_t *);

int
main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s file\n", getprogname());
		exit(EXIT_FAILURE);
	}

	size_t n;
	int rv = EXIT_SUCCESS;
	const char *s = readfile(argv[1], &n);

	clock_t tmbeg = clock();
	printf("%d: ", unicode_generic(s, n));
	printf("Elapsed time: %.3fs\n", (double)(clock() - tmbeg) / CLOCKS_PER_SEC);

	tmbeg = clock();
	printf("%d: ", unicode_simd(s, n));
	printf("Elapsed time: %.3fs\n", (double)(clock() - tmbeg) / CLOCKS_PER_SEC);

	return rv;
}

int
unicode_generic(const char *s, size_t n)
{
	int acc = 0;
	while (n > 0) {
		rune ch;
		int w = u8tor(&ch, s);
		acc += stage2[stage1[ch / CCCBLKSIZ]][ch % CCCBLKSIZ];
		s += w;
		n -= w;
	}
	return acc;
}

int
unicode_simd(const char *s, size_t n)
{
	int acc = 0;
	const __m512i modmsk = _mm512_set1_epi32(CCCBLKSIZ - 1);

	while (n > 0) {
		int w = 0;
		alignas(64) uint32_t data[16];

		w += u8tor(&data[ 0], s + w);
		w += u8tor(&data[ 1], s + w);
		w += u8tor(&data[ 2], s + w);
		w += u8tor(&data[ 3], s + w);
		w += u8tor(&data[ 4], s + w);
		w += u8tor(&data[ 5], s + w);
		w += u8tor(&data[ 6], s + w);
		w += u8tor(&data[ 7], s + w);
		w += u8tor(&data[ 8], s + w);
		w += u8tor(&data[ 9], s + w);
		w += u8tor(&data[10], s + w);
		w += u8tor(&data[11], s + w);
		w += u8tor(&data[12], s + w);
		w += u8tor(&data[13], s + w);
		w += u8tor(&data[14], s + w);
		w += u8tor(&data[15], s + w);

		s += w;
		n -= w;

		__m512i vec = _mm512_load_epi32(data);
		__m512i vidx = _mm512_srli_epi32(vec, CCCBLKSIZ_LOG2); /* vidx = vec / CCCBLKSZ */
#if USE_GATHER
		vidx = _mm512_i32gather_epi32(vidx, stage1, sizeof(rune));
#else
		_mm512_store_epi32(data, vidx);
		for (int i = 0; i < 16; i++)
			data[i] = stage1[data[i]];
		vidx = _mm512_load_epi32(data);
#endif
		vidx = _mm512_slli_epi32(vidx, CCCBLKSIZ_LOG2);
		vec = _mm512_and_epi32(vec, modmsk);
		vidx = _mm512_add_epi32(vidx, vec);
#if USE_GATHER
		vec = _mm512_i32gather_epi32(vidx, stage2, sizeof(rune));
		_mm512_store_epi32(data, vec);
#else
		_mm512_store_epi32(data, vidx);
		for (int i = 0; i < 16; i++)
			data[i] = ((uprop_ccc_t *)stage2)[data[i]];
#endif

		for (int i = 0; i < 16; i++)
			acc += (uprop_ccc_t)data[i];
	}

	return acc;
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
