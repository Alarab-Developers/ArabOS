#include "مكتبة_معالجة_النصوص.h"

int strcmp(const char* a, const char* b)
{
    int i = 0;

    while (a[i] && b[i]) {

        if (a[i] != b[i])
            return a[i] - b[i];

        i++;
    }

    return a[i] - b[i];
}

void strcpy(char* dest, const char* src)
{
    int i = 0;

    while (src[i]) {
        dest[i] = src[i];
        i++;
    }

    dest[i] = 0;
}

int strlen(const char* s)
{
    int i = 0;

    while (s[i])
        i++;

    return i;
}

void* memset(void* dest, int val, unsigned long size) {
    unsigned char* d = dest;
    for (unsigned long i = 0; i < size; i++) {
        d[i] = (unsigned char)val;
    }
    return dest;
}


void* memcpy(void* dest, const void* src, unsigned long size) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;

    for (unsigned long i = 0; i < size; i++) {
        d[i] = s[i];
    }

    return dest;
}
