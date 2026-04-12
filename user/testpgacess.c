#include "kernel/types.h"
#include "user/user.h"

void print_bitmask(unsigned int mask, int n) {
    for (int i = n - 1; i >= 0; i--) {
        if (mask & (1 << i)) {
            printf("1");
        } else {
            printf("0");
        }
        
        if (i % 4 == 0 && i != 0) printf(" ");
    }
    printf("\n");
}

int main()
{
    char *buf = malloc(32 * 4096); // Cấp phát 32 trang
  // User giả vờ truy cập một số trang
  buf[0 * 4096] = 'a'; // Truy cập trang 0
  buf[2 * 4096] = 'b'; // Truy cập trang 2
  buf[30 * 4096] = 'c'; // Truy cập trang 30
    int mask = 0;

    pgacess(buf , mask);
    
    print_bitmask(mask, 32);
    exit(0);
}