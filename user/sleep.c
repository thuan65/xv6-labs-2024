#include "kernel/types.h"
#include "user/user.h"

int isValidInt(char *s) {
    if (s[0] == '\0') return 0;
    for (int i = 0; s[i] != '\0'; i++)
        if (s[i] < '0' || s[i] > '9')
            return 0;
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(2, "Need two arguments only\n");
        exit(1);
    }
    if (!isValidInt(argv[1])) {
        fprintf(2, "Not a valid number\n");
        exit(1);
    }
    int count = atoi(argv[1]);
    printf("Sleeping for %d ticks\n", count);
    sleep(count);
    printf("Woke up\n");
    exit(0);
}