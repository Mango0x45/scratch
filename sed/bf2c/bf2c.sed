#!/usr/bin/sed -f

1i\
#include <err.h>\
#include <stdint.h>\
#include <stdio.h>\
#include <stdlib.h>\
int \
main(void)\
{\
	uint8_t *p = calloc(16384, 1);\
	if (!p) \
		err(EXIT_FAILURE, "calloc");

/./!d

s/[^][+<>.,-]//g
s/\[[-+]\]/*p = 0;/g
s/[-+]/(*p)&&;/g
s/</p--;/g
s/>/p++;/g
s/\./putchar(*p);/g
s/,/*p = (uint8_t)getchar();/g
s/\[/while (*p) {/g
s/\]/}/g

$a}
