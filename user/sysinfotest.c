#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/sysinfo.h"
#include "user/user.h"

void
fail(char *s)
{
  printf("sysinfotest: %s failed\n", s);
  exit(1);
}

void
test_mem(void)
{
  struct sysinfo before;
  struct sysinfo after;
  char *p;

  if(sysinfo(&before) < 0)
    fail("sysinfo before alloc");

  p = sbrk(4096);
  if((uint64)p == (uint64)-1)
    fail("sbrk alloc");


  p[0] = 1;

  if(sysinfo(&after) < 0)
    fail("sysinfo after alloc");

  if(after.freemem >= before.freemem)
    fail("freemem did not decrease");

  if(sbrk(-4096) == (char*)-1)
    fail("sbrk free");
}

void
test_proc(void)
{
  struct sysinfo before;
  struct sysinfo during;
  int pid;

  if(sysinfo(&before) < 0)
    fail("sysinfo before fork");

  pid = fork();
  if(pid < 0)
    fail("fork");

  if(pid == 0){
    sleep(50);
    exit(0);
  }

  if(sysinfo(&during) < 0)
    fail("sysinfo after fork");

  if(during.nproc < before.nproc + 1)
    fail("nproc did not increase");

  wait(0);
}

int
main(void)
{
  test_mem();
  test_proc();
  printf("sysinfotest: OK\n");
  exit(0);
}
