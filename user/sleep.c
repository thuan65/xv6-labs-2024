#include "kernel/types.h"
#include "user/user.h"


int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Need two argument only!");
        exit(0);
    }

    printf("Sleeping for a little while\n");
    sleep(atoi(argv[1]));
    printf("woke up\n");
    exit(0);
}