#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char *argv[]) {

    int i;
    char *nargv[MAXARG];

    if(argc < 3 || (argv[1][0] < '0' || argv[1][0] > '9')){
        fprintf(2, "Usage: trace bitmask command [args]\n");
        exit(1);
    }

    // validate argv[1] is a valid number
    for(i = 0; argv[1][i] != '\0'; i++){
        if(argv[1][i] < '0' || argv[1][i] > '9'){
            fprintf(2, "trace: bitmask must be a valid number\n");
            exit(1);
        }
    }
    int mask = atoi(argv[1]);

    // call syscall trace with the bitmask provided
    if (trace(mask) < 0) {
        fprintf(2, "trace: trace failed\n");
        exit(1);
    }

    // prepare the second command
    for(i = 2; i < argc && i < MAXARG; i++){
        nargv[i-2] = argv[i];
    }
    nargv[i-2] = 0;

    exec(nargv[0], nargv);

    // if exec succeeds, the code below is never reached
    fprintf(2, "exec %s failed\n", nargv[0]);
    exit(1);
}