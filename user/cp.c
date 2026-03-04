#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[512];

int main(int argc, char *argv[]) {
    int fd0, fd1, n;

    if (argc <= 2) {
        printf("usage: cp src dst\n");
        exit(1);
    }

    if ((fd0 = open(argv[1], O_RDONLY)) < 0) {
        printf("cp: cannot open %s\n", argv[1]);
        exit(1);
    }

    if ((fd1 = open(argv[2], O_CREATE|O_RDWR)) < 0) {
        printf("cp: cannot open %s\n", argv[2]);
        exit(1);
    }

    while ((n = read(fd0, buf, sizeof(buf))) > 0) 
    {
        write(fd1, buf, n);
    }
    close(fd0);
    close(fd1);

    exit(0);
}