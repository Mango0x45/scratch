#ifndef SHA1_SHA1_H
#define SHA1_SHA1_H

#include <stddef.h>
#include <stdint.h>

#define SHA1DGSTSZ 20
#define SHA1BLKSZ  64

typedef struct {
	uint32_t dgst[5];
	uint64_t msgsz;
	uint8_t buf[64];
	size_t bufsz;
} sha1_t;

void sha1init(sha1_t *);
int sha1hash(sha1_t *, const uint8_t *, size_t);
void sha1end(sha1_t *, uint8_t[SHA1DGSTSZ]);

#endif /* !SHA1_SHA1_H */
