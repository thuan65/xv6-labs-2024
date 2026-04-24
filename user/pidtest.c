#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h" // Chứa khai báo ugetpid() và getpid()

void test_speed() {
  int n = 100000; // Số lần lặp
  int start, end;
  int pid;

  printf("\n--- Bat dau do toc do (vong lap %d lan) ---\n", n);

  // 1. Do toc do getpid() truyen thong
  start = uptime();
  for (int i = 0; i < n; i++) {
    pid = getpid();
  }
  end = uptime();
  printf("getpid() (System Call): %d ticks\n", end - start);

  // 2. Do toc do ugetpid() "sieu toc"
  start = uptime();
  for (int i = 0; i < n; i++) {
    pid = ugetpid();
  }
  end = uptime();
  printf("ugetpid() (Shared Memory): %d ticks\n", end - start);
  
  // Tranh compiler toi uu hoa mat bien pid
  (void)pid; 
}

int
main(void)
{
  int real_pid = getpid();
  int fast_pid = ugetpid();

  printf("PID Check: System=%d, Shared=%d\n", real_pid, fast_pid);

  if (real_pid == fast_pid) {
    printf("SUCCESS: PID matches!\n");
    test_speed();
  } else {
    printf("FAILED: PID mismatch!\n");
  }

  exit(0);
}