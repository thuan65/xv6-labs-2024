#include "kernel/types.h"
#include <stdbool.h>
#include "user/user.h"



void concurrent_prime(int* fd) {

    int num;
    if (read(fd[0], &num, sizeof(num)) == 0) {
        close(fd[0]);
        return;
    }
    printf("prime %d\n", num);

    int p[2]; //The pipe of this process
    pipe(p);

    int tmp = -1;

    while(true) {

        if (read(fd[0], &tmp, sizeof(tmp)) == 0) {
            break;
        }
        if (tmp % num != 0) {
            write(p[1], &tmp, sizeof(tmp));
        }
    }

    close(fd[0]);
    close(p[1]);

    if (tmp == -1) {
        close(p[0]);
        return;
    }

    if (fork() == 0) {
        close(p[1]);
        concurrent_prime(p);
        close(p[0]);
    }
    else{
        close(p[0]);
        wait(0);
    }

}

int main() {

    int fd[2];
    pipe(fd); // fd[0] : read, fd[1]: write

    if (fork() == 0) {
        close(fd[1]);
        concurrent_prime(fd);
        close(fd[0]);
    }
    else {
        for (int i = 2; i <= 250; ++ i) {
                write(fd[1], &i, sizeof(i));
        }
        close(fd[1]);
        wait(0);
    }
    

   

    exit(0);
}