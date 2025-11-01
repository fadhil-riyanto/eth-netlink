#include <stdio.h>
#include <sys/types.h>

void __vt_hexdump_color(char *fmt, u_int8_t vt_hexdump_hex) {
        u_int8_t current_hex = vt_hexdump_hex;

        if (current_hex == 0x7F) {
                printf("\033[1;31m %02x \033[0m", vt_hexdump_hex);
        } else if (current_hex == 0xFF) {
                printf("\033[1;34m %02x \033[0m", vt_hexdump_hex);
        } else if (current_hex == 0x00) {
                printf("\033[1;37m %02x \033[0m", vt_hexdump_hex);
        } else {
                printf("\033[1;32m %02x \033[0m", vt_hexdump_hex);
        }
}

void __vt_title(unsigned long ptr, size_t t_ptr_size) {
        unsigned long t_realptr = ptr;
        printf("================= VT_HEXDUMP =================\n");
        printf("file\t\t: %s:%d\n", __FILE__, __LINE__);
        printf("func\t\t: %s\n", __FUNCTION__);
        printf("addr\t\t: 0x%064lx\n", t_realptr);
        printf("dump_size\t: %ld\n\n", t_ptr_size);
}

void vt_hexdump(unsigned long ptr, size_t t_ptr_size) {
        __vt_title(ptr, t_ptr_size);
}

int main() {
        char *buf = "Test";

        vt_hexdump(()buf, 4);
}