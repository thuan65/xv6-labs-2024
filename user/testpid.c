#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main (int argc, char* argv[])
{
    int pid = getpid();

    int upid = ugetpid();


    printf("Normal getpid(): %d\n", pid);
    printf("Fast ugetpid():   %d\n", upid);

    exit(0);
}