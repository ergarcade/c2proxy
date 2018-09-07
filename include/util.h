#ifndef UTIL_H
#define UTIL_H

class util {
    public:
        static void hexdump(unsigned char *buff, unsigned int len);

    private:
        static void hexdump_readable(unsigned char *b, unsigned char *s);
};

#endif // UTIL_H
