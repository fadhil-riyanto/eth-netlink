#include <stdio.h>

int main() {
    unsigned long a[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',  'k', 'l', 'm', '\n', '\0'};
    int n = sizeof(a) / sizeof(a[0]);
     
    fwrite(a, 2, n, stdout);
    return 0;
}

// abcdefghijklm
// abcdefgh
// abcd