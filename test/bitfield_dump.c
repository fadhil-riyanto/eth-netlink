#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#ifndef VT_BITFIELD_DUMP
#define VT_BITFIELD_DUMP(x)                                                    \
        do {                                                                   \
                u_int8_t mod = 8;                                              \
                u_int8_t nstart = 1;                                           \
                u_int8_t size = sizeof(x) * 8;                                 \
                char status[20];                                               \
                                                                               \
                for (u_int8_t i = 1; i < mod + 1; i++) {                       \
                        nstart = i;                                            \
                        for (u_int8_t ii = 0; ii < (size / mod); ii++) {       \
                                u_int8_t temp = (x >> nstart - 1) & 0x1;       \
                                if (temp == 1)                                 \
                                        strcpy(status, "\x1b[33mtrue\x1b[0m"); \
                                else                                           \
                                        strcpy(status, "false");               \
                                                                               \
                                (nstart < 9)                                   \
                                    ? printf("%d = %s\t ", nstart, status)     \
                                    : printf("%d = %s\t", nstart, status);     \
                                nstart = nstart + mod;                         \
                        }                                                      \
                        printf("\n");                                          \
                }                                                              \
        } while (0);

#endif

int main() {
        unsigned int x = 0x7cff;
        VT_BITFIELD_DUMP(x);
}