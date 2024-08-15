#include <endian.h>
#include <errno.h>
#include <string.h>

#include "sha1.h"

#define lengthof(xs) (sizeof(xs) / sizeof(*(xs)))
#define MIN(x, y)    ((x) < (y) ? (x) : (y))

static void sha1hashblk(sha1_t *, const uint8_t *);

static const uint32_t K[] = {
	0x5A827999,
	0x6ED9EBA1,
	0x8F1BBCDC,
	0xCA62C1D6,
};

static inline uint32_t
rotl32(uint32_t x, uint8_t bits)
{
#if (__GNUC__ || __TINYC__) && __x86_64__
	asm ("roll %1, %0" : "+r" (x) : "c" (bits) : "cc");
	return x;
#else
	return (x << bits) | (x >> (32 - bits));
#endif
}

void
sha1init(sha1_t *s)
{
	static const uint32_t H[] = {
		0x67452301,
		0xEFCDAB89,
		0x98BADCFE,
		0x10325476,
		0xC3D2E1F0,
	};
	memcpy(s->dgst, H, sizeof(H));
	s->msgsz = s->bufsz = 0;
}

int
sha1hash(sha1_t *s, const uint8_t *msg, size_t msgsz)
{
	if (s->msgsz + (msgsz * 8) < s->msgsz)
		return EOVERFLOW;

	s->msgsz += msgsz * 8;

	while (msgsz != 0) {
		size_t free_space = SHA1BLKSZ - s->bufsz;
		size_t ncpy = MIN(msgsz, free_space);
		memcpy(s->buf + s->bufsz, msg, ncpy);
		s->bufsz += ncpy;
		msg += ncpy;
		msgsz -= ncpy;

		if (s->bufsz == SHA1BLKSZ) {
			sha1hashblk(s, s->buf);
			s->bufsz = 0;
		}
	}

	return 0;
}

void
sha1end(sha1_t *s, uint8_t dgst[SHA1DGSTSZ])
{
	s->buf[s->bufsz++] = 0x80;

	if (s->bufsz > SHA1BLKSZ - sizeof(uint64_t)) {
		while (s->bufsz < SHA1BLKSZ)
			s->buf[s->bufsz++] = 0;
		sha1hashblk(s, s->buf);
		s->bufsz = 0;
	}

	while (s->bufsz < 56)
		s->buf[s->bufsz++] = 0;
	((uint64_t *)s->buf)[SHA1BLKSZ/8 - 1] = htobe64(s->msgsz);

	sha1hashblk(s, s->buf);

	for (int i = 0; i < lengthof(s->dgst); i++)
		((uint32_t *)dgst)[i] = htobe32(s->dgst[i]);
}

static void
sha1hashblk(sha1_t *s, const uint8_t *blk)
{
	uint32_t w[80];
	uint32_t a, b, c, d, e, tmp;

	for (int i = 0; i < 16; i++)
		w[i] = htobe32(((uint32_t *)blk)[i]);
	for (int i = 16; i < 32; i++)
		w[i] = rotl32(w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16], 1);
	for (int i = 32; i < 80; i++)
		w[i] = rotl32(w[i-6] ^ w[i-16] ^ w[i-28] ^ w[i-32], 2);

	a = s->dgst[0];
	b = s->dgst[1];
	c = s->dgst[2];
	d = s->dgst[3];
	e = s->dgst[4];

	for (int i = 0; i < 80; i++) {
		uint32_t f, k;

		if (i < 20) {
			f = b&c | ~b&d;
			k = K[0];
		} else if (i < 40) {
			f = b ^ c ^ d;
			k = K[1];
		} else if (i < 60) {
			f = b&c | b&d | c&d;
			k = K[2];
		} else {
			f = b ^ c ^ d;
			k = K[3];
		}

		tmp = rotl32(a, 5) + f + e + w[i] + k;
		e = d;
		d = c;
		c = rotl32(b, 30);
		b = a;
		a = tmp;
	}

	s->dgst[0] += a;
	s->dgst[1] += b;
	s->dgst[2] += c;
	s->dgst[3] += d;
	s->dgst[4] += e;
}
