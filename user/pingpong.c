#include "kernel/types.h"
#include "user/user.h"

int main() {
    //There are 2 file descriptors
    int fd1[2]; 
    int fd2[2];

    char buffer[20]; // This is for hold the content

    //Create Pipe ==fd[0]: READ, fd[1]: write==
    pipe(fd1); //Child To Parent
    pipe(fd2); //Parent to Child


    
    if (fork() == 0) {//Tien trinh con

        //
        close(fd2[1]); //Close the write head
        read(fd2[0], buffer, sizeof(buffer));
        printf("Child received: %s\n", buffer);
        close(fd2[0]);

        close(fd1[0]);
        char message[] = "pong";
        write(fd1[1], message, sizeof(message));
        close(fd1[1]);

    }
    else {//The Father
        close(fd2[0]);// Close the read head pipe 2
        char message[] = "ping";
        write(fd2[1], message , sizeof(message));
        close(fd2[1]);

        close(fd1[1]);
        read(fd1[0], buffer, sizeof(buffer));
        printf("Parent received: %s\n", buffer);
        close(fd1[0]);
    }

    exit(0);
}