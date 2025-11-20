#include "utils.h"
#include <stdio.h>


void print_bool(const char* buf, u_int8_t data)
{
        if ((data & 1) == 1) {
                printf("%s", buf);
        };
}