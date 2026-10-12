#include "types.h"
#include "stat.h"
#include "user.h"

#define NUM_THREADS 3
#define INCREMENTS 100000

volatile int counter = 0;
int mutex_sem;

void worker(void *arg) {
  for(int i = 0; i < INCREMENTS; i++) {
    sem_wait(mutex_sem);   // Critical section entry
    counter++;
    sem_post(mutex_sem);   // Critical section exit
  }
  exit();
}

int main(int argc, char *argv[]) {
  void *stacks[NUM_THREADS];

  // Initialize binary semaphore with value = 1
  mutex_sem = sem_init(1);
  if(mutex_sem < 0) {
    printf(1, "failed to initialize semaphore\n");
    exit();
  }

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

  printf(1, "Synchronized Counter Value: %d (Expected: %d)\n", 
         counter, NUM_THREADS * INCREMENTS);

  for(int i = 0; i < NUM_THREADS; i++) {
    free(stacks[i]);
  }

  exit();
}