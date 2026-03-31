#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {

    if (argc < 3) {
        fprintf(2, "cp: too few arguments\n");
        fprintf(2, "usage: cp <src> <dst>\n");
        exit(1);
    } else if (argc > 3) {
        fprintf(2, "cp: too many arguments\n");
        fprintf(2, "usage: cp <src> <dst>\n");
        exit(1);
    }

    if (strcmp(argv[1], argv[2]) == 0) {
        fprintf(2, "cp: source and destination are the same\n");
        fprintf(2, "usage: cp <src> <dst>\n");
        exit(1);
    }

    int fd_src;
    int fd_dst;

    if ((fd_src = open(argv[1], O_RDONLY)) < 0) {
        fprintf(2, "cp: cannot open %s\n", argv[1]);
        exit(1);
    }
    
    if ((fd_dst = open(argv[2], O_WRONLY | O_CREATE | O_TRUNC)) < 0) {
        fprintf(2, "cp: cannot open %s\n", argv[2]);
        close(fd_src);
        exit(1);
    }

    char buffer[1024];
    int n;

    while((n = read(fd_src, buffer, sizeof(buffer))) > 0) {
        if (write(fd_dst, buffer, n) != n) {
            fprintf(2, "cp: write failed: %s\n", argv[2]);
            close(fd_src);
            close(fd_dst);
            exit(1);
        }
    }

    if (n < 0) {
        fprintf(2, "cp: read failed: %s\n", argv[1]);
        close(fd_src);
        close(fd_dst);
        exit(1);
    }

    close(fd_src);
    close(fd_dst);
    exit(0);
}