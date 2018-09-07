#include <stdio.h>

#include "util.h"

#define HEXDUMP_WIDTH   16

/*
 * Dump ascii part of a hexdump line.
 */
void util::hexdump_readable(unsigned char *b, unsigned char *s) {
#ifndef QT_NO_DEBUG_OUTPUT
    int i = 3 * (HEXDUMP_WIDTH - (b - s));

    if (s == b) {
        return;
    }

    fprintf(stderr, "    ");
    while (i--) {
        fprintf(stderr, " ");
    }

    for (; s != b; s++) {
        fprintf(stderr, "%c", (*s >= 0x20 && *s <= 0x7e) ? *s : '.');
    }

    fprintf(stderr, "\n");
#else
    (void)b;
    (void)s;
#endif
}

/*
 * Dump a buffer in hexdump / ascii format.
 */
void util::hexdump(unsigned char *buff, unsigned int len) {
#ifndef QT_NO_DEBUG_OUTPUT
    int i;
    unsigned char *b;

    for (b = buff, i = 0; b != &buff[len]; ) {
        fprintf(stderr, "%02x ", *b++);
        if (++i == HEXDUMP_WIDTH) {
            hexdump_readable(b, b - HEXDUMP_WIDTH);
            i = 0;
        }
    }

    hexdump_readable(b, b - (len % HEXDUMP_WIDTH));
#else
    (void)buff;
    (void)len;
#endif
}
