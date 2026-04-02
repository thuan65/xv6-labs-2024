#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  printf("Starting fork test...\n");

  int pid = fork();
  if(pid < 0){
    printf("fork failed\n");
    exit(1);
  }

  if(pid == 0){
    // Child process
    printf("Child (pid %d) is forking again...\n", getpid());
    fork();
  } else {
    // Parent process
    wait(0);
    wait(0);
  }

  exit(0);
}