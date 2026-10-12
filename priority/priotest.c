#include "types.h"
#include "stat.h"
#include "user.h"

void cpu_bound_work(int iterations) {
  volatile int x = 0;
  (void)x;
  for (int i = 0; i < iterations; i++) {
    x += (i * 3) % 7;
  }
}

int main(void) {
  int pids[4];
  (void)pids; // To avoid unused variable warning if not used
  int initial_priorities[4] = {20, 15, 10, 5}; // lowest to highest priority

  printf(1, "starting priority scheduler + aging test...\n");

  for (int i = 0; i < 4; i++) {
    int pid = fork();
    if (pid < 0) {
      printf(1, "fork failed!\n");
      exit();
    } else if (pid == 0) {
      int my_pid = getpid();
      setpriority(my_pid, initial_priorities[i]);
      
      printf(1, "child PID %d started with initial priority: %d\n", my_pid, initial_priorities[i]);
      
      // CPU-bound loop
      for (int k = 0; k < 5; k++) {
        cpu_bound_work(20000000);
      }
      
      printf(1, "--> child PID %d FINISHED! final priority: %d\n", 
             my_pid, getpriority(my_pid));
      exit();
    } else {
      pids[i] = pid;
    }
  }

  for (int i = 0; i < 4; i++) {
    int finished_pid = wait();
    printf(1, "parent collected child PID %d\n", finished_pid);
  }

  printf(1, "all processes finished successfully.\n");
  exit();
}