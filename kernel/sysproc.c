#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "ptree.h"
#include "sysinfo.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}


//Assume max here stand for the maxium process that the user want to see
uint64 sys_ptree(void) {
  int max;
  argint(1, &max);
  if (max <= 0) return -1;
  
  uint64 addr;
  argaddr(0, &addr);

  static struct ptreeinfo pbuff[NPROC];

  int total_count = get_proc_info(pbuff, max);
  int ret_count = (total_count > max) ? max : total_count; //The actual number of process in pbuff

  if (total_count > max) {
    printf("Warning: Data truncated! Showing %d of %d processes.\n", max, total_count);
  }
 

  struct proc *p = myproc();
  if (copyout(p->pagetable, addr, (char*)& pbuff, sizeof(struct ptreeinfo) * ret_count) < 0) {
    return -1;  
  }

  return ret_count;
}
uint64
sys_sysinfo(void)
{
  uint64 addr;
  struct proc *p = myproc();
  struct sysinfo info;

  argaddr(0, &addr);
  info.freemem = freemem();
  info.nproc = nproc();

  if(copyout(p->pagetable, addr, (char *)&info, sizeof(info)) < 0)
    return -1;
  return 0;
}

uint64 sys_trace(void) {
  int bitmask;
  argint(0, &bitmask);
  myproc()->trace_mask = bitmask;
  return 0;
}

uint64
sys_pgaccess(void)
{
  //Virtual address of the start of the page
  uint64 pg_address;
  argaddr(0, &pg_address);

  int number_of_page;
  argint(1, &number_of_page);

  //address of the buffer bitmask
  uint64 addr;
  argaddr(2, &addr);

  uint32 mask = 0;
  struct proc *p = myproc();

  //function in file vm.c
  pgaccess(p->pagetable, pg_address, number_of_page, &mask);

  if(copyout(p->pagetable, addr, (char *)&mask, sizeof(mask)) < 0) {
    return -1;
  }

  return 0;
}