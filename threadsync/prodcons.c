#include "types.h"
#include "stat.h"
#include "user.h"

#define BUFFER_SIZE 5
#define TOTAL_ITEMS 20

int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;

int mutex;
int empty;
int full;

void producer(void *arg) {
  for(int i = 1; i <= TOTAL_ITEMS; i++) {
    sem_wait(empty); // Wait for an empty slot
    sem_wait(mutex); // Acquire buffer mutex

    buffer[in] = i;
    printf(1, "Producer: Produced item %d at slot %d\n", i, in);
    in = (in + 1) % BUFFER_SIZE;

    sem_post(mutex); // Release buffer mutex
    sem_post(full);  // Signal new item available
  }
  exit();
}

void consumer(void *arg) {
  for(int i = 1; i <= TOTAL_ITEMS; i++) {
    sem_wait(full);  // Wait for an item
    sem_wait(mutex); // Acquire buffer mutex

    int item = buffer[out];
    printf(1, "Consumer: Consumed item %d from slot %d\n", item, out);
    out = (out + 1) % BUFFER_SIZE;

    sem_post(mutex); // Release buffer mutex
    sem_post(empty); // Signal free slot available
  }
  exit();
}

int main(int argc, char *argv[]) {
  void *stack_p = malloc(4096);
  void *stack_c = malloc(4096);

  // Initialize counting semaphores
  mutex = sem_init(1);           // Binary mutex
  empty = sem_init(BUFFER_SIZE); // 5 empty slots
  full  = sem_init(0);           // 0 full slots

  if(mutex < 0 || empty < 0 || full < 0) {
    printf(1, "semaphore initialization failed\n");
    exit();
  }

  printf(1, "Starting Producer-Consumer (Buffer Size: %d)...\n", BUFFER_SIZE);

  int p1 = clone(producer, 0, stack_p);
  int p2 = clone(consumer, 0, stack_c);

  if(p1 < 0 || p2 < 0) {
    printf(1, "clone failed for producer or consumer\n");
    exit();
  }

  join();
  join();

  printf(1, "Producer-Consumer finished successfully!\n");

  free(stack_p);
  free(stack_c);
  exit();
}