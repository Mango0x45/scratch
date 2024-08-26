#include <sys/mman.h>
#include <sys/stat.h>

#include <err.h>
#include <fcntl.h>
#include <immintrin.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <bsd/stdlib.h>

static const unsigned char *readfile(const char *, size_t *);

bool
strisascii_avx512(const unsigned char *s, size_t n)
{
	__m512i msk = _mm512_set1_epi8((char)(1 << 7));
	while (n >= sizeof(__m512i)) {
		if (_mm512_test_epi8_mask(_mm512_loadu_epi8(s), msk) != 0)
			return false;
		s += sizeof(__m512i);
		n -= sizeof(__m512i);
	}
	for (size_t i = 0; i < n; i++) {
		if (s[i] > 0x7F)
			return false;
	}
	return true;
}

bool
strisascii_avx2(const unsigned char *s, size_t n)
{
	__m256i msk = _mm256_set1_epi8((char)(1 << 7));
	while (n >= sizeof(__m256i)) {
		if (_mm256_testz_si256(_mm256_loadu_si256((__m256i *)s), msk) == 0)
			return false;
		s += sizeof(__m256i);
		n -= sizeof(__m256i);
	}
	for (size_t i = 0; i < n; i++) {
		if (s[i] > 0x7F)
			return false;
	}
	return true;
}

bool
strisascii_dumb(const unsigned char *s, size_t n)
{
	for (size_t i = 0; i < n; i++) {
		if (s[i] > 0x7F)
			return false;
	}
	return true;
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
	const unsigned char *beg = readfile(argv[1], &len);

	clock_t tmbeg = clock();
	if (!strisascii_avx512(beg, len))
		puts("Non-ASCII");
	printf("Elapsed time (AVX-512): %.3fs\n", (double)(clock() - tmbeg) / CLOCKS_PER_SEC);

	tmbeg = clock();
	if (!strisascii_avx2((const unsigned char *)beg, len))
		puts("Non-ASCII");
	printf("Elapsed time (AVX-2):   %.3fs\n", (double)(clock() - tmbeg) / CLOCKS_PER_SEC);

	tmbeg = clock();
	if (!strisascii_dumb((const unsigned char *)beg, len))
		puts("Non-ASCII");
	printf("Elapsed time (Generic): %.3fs\n", (double)(clock() - tmbeg) / CLOCKS_PER_SEC);

	munmap((void *)beg, len);
	return rv;
}

const unsigned char *
readfile(const char *filename, size_t *n)
{
	int fd = open(filename, O_RDONLY);
	if (fd == -1)
		err(1, "open: %s", filename);

	struct stat sb;
	if (fstat(fd, &sb) == -1)
		err(1, "fstat: %s", filename);

	*n = sb.st_size;
	const unsigned char *p = mmap(NULL, *n, PROT_READ, MAP_PRIVATE, fd, 0);
	if (p == MAP_FAILED)
		err(1, "mmap: %s", filename);
	return p;
}
