#include <stdio.h>
#include "utils.h"

#define HEXDUMP_WIDTH   16

/*
 * Dump ascii part of a hexdump line.
 */
static void hexdump_readable(unsigned char *b, unsigned char *s) {
    int i = 3 * (HEXDUMP_WIDTH - (b - s));

    if (s == b) {
        return;
    }

    printf("    ");
    while (i--) {
        printf(" ");
    }

    for (; s != b; s++) {
        printf("%c", (*s >= 0x20 && *s <= 0x7e) ? *s : '.');
    }

    printf("\n");
}

/*
 * Dump a buffer in hexdump / ascii format.
 */
void hexdump(unsigned char *buff, unsigned int len) {
    int i;
    unsigned char *b;

    for (b = buff, i = 0; b != &buff[len]; ) {
        printf("%02x ", *b++);
        if (++i == HEXDUMP_WIDTH) {
            hexdump_readable(b, b - HEXDUMP_WIDTH);
            i = 0;
        }
    }

    hexdump_readable(b, b - (len % HEXDUMP_WIDTH));
}

