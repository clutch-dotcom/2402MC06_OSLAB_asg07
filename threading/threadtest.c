#include "types.h"
#include "stat.h"
#include "user.h"

#define NUM_THREADS 3
#define INCREMENTS 100000

volatile int counter = 0;

void worker(void *arg) {
  for(int i = 0; i < INCREMENTS; i++) {
    int temp = counter;
    sleep(0); // Yield to other threads
    counter = temp + 1;
  }
  exit();
}

int main(int argc, char *argv[]) {
  void *stacks[NUM_THREADS];

  for(int i = 0; i < NUM_THREADS; i++) {
    stacks[i] = malloc(4096);
    if(stacks[i] == 0) {
      printf(1, "malloc failed\n");
      exit();
    }

    int pid = clone(worker, 0, stacks[i]);
    if(pid < 0) {
      printf(1, "clone failed\n");
      exit();
    }
  }

  for(int i = 0; i < NUM_THREADS; i++) {
    join();
  }

  printf(1, "Final counter value: %d\n", counter);

  for(int i = 0; i < NUM_THREADS; i++) {
    free(stacks[i]);
  }

  exit();
}