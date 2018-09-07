#include <stdio.h>

typedef struct msg_st {
    unsigned char buff[100];
    unsigned int len;
    unsigned char expected_cksum;
} msg_t;

unsigned char checksum(unsigned char *b, unsigned int len) {
    unsigned char cksum = 0;
    while (len--) {
        cksum ^= *b++;
    }
    return cksum;
}

void print_checksum(msg_t *m) {
    int i;
    unsigned char calced = checksum(m->buff, m->len);

    if (calced != m->expected_cksum) {
        printf("failed check: ");
    } else {
        printf("              ");
    }
    for (i = 0; i < m->len; i++) {
        printf("%02x ", m->buff[i]);
    }
    printf(": %02x expected %02x\n", checksum(m->buff, m->len), m->expected_cksum);
}

int main(void) {
    unsigned int i;

    msg_t msgs[] = {
        {
            { 0x81, 0x1a, 0x07, 0xa0, 0x05, 0x98, 0x3a, 0x00, 0x00, 0x55 },
            10,
            0x4f
        },
        { { 0x01, 0x05, 0x80, 0x02, 0x00, 0x01 }, 6, 0x87 },
        { { 0x80 }, 1, 0x80 },
        { { 0x01, 0x80, 0x01, 0x01 }, 4, 0x81 },
        { { 0x1a, 0x01, 0xa0 }, 3, 0xbb },
        { { 0x1a, 0x07, 0x05, 0x05, 0x80, 0x64, 0x00, 0x00, 0x00 }, 9, 0xf9 },
        { { 0x81, 0x1a, 0x01, 0x05 }, 4, 0x1e }
    };

    for (i = 0; i < sizeof (msgs) / sizeof(msg_t); i++) {
        print_checksum(&msgs[i]);
    }

    return 0;
}
