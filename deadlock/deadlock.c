#include "types.h"
#include "stat.h"
#include "user.h"

#define MAX_P 5
#define MAX_R 3

// Matrix state structures
int N = 5; // Processes
int M = 3; // Resources

void print_matrices(int Allocation[MAX_P][MAX_R], int Need[MAX_P][MAX_R], int Available[MAX_R]) {
  printf(1, "\nAvailable Resources: ");
  for(int j = 0; j < M; j++) {
    printf(1, "%d ", Available[j]);
  }
  printf(1, "\n\nProc\tAllocation\tNeed/Request\n");
  for(int i = 0; i < N; i++) {
    printf(1, "P%d\t", i);
    for(int j = 0; j < M; j++) printf(1, "%d ", Allocation[i][j]);
    printf(1, "\t\t");
    for(int j = 0; j < M; j++) printf(1, "%d ", Need[i][j]);
    printf(1, "\n");
  }
  printf(1, "\n");
}

// ==========================================
// Task (b): Deadlock Detection Algorithm
// ==========================================
int detect_deadlock(int Allocation[MAX_P][MAX_R], int Request[MAX_P][MAX_R], int Available[MAX_R]) {
  int Work[MAX_R];
  int Finish[MAX_P];
  int deadlocked[MAX_P];
  int deadlocked_count = 0;

  for(int j = 0; j < M; j++) Work[j] = Available[j];
  for(int i = 0; i < N; i++) {
    Finish[i] = 0;
    // If process has no allocation, it cannot cause deadlock
    int has_allocation = 0;
    for(int j = 0; j < M; j++) {
      if(Allocation[i][j] != 0) has_allocation = 1;
    }
    if(!has_allocation) Finish[i] = 1;
  }

  int count = 0;
  while(count < N) {
    int found = 0;
    for(int i = 0; i < N; i++) {
      if(!Finish[i]) {
        int can_satisfy = 1;
        for(int j = 0; j < M; j++) {
          if(Request[i][j] > Work[j]) {
            can_satisfy = 0;
            break;
          }
        }

        if(can_satisfy) {
          for(int j = 0; j < M; j++) Work[j] += Allocation[i][j];
          Finish[i] = 1;
          found = 1;
          count++;
        }
      }
    }
    if(!found) break; // Deadlock detected
  }

  // Identify deadlocked processes
  for(int i = 0; i < N; i++) {
    if(!Finish[i]) {
      deadlocked[deadlocked_count++] = i;
    }
  }

  if(deadlocked_count > 0) {
    printf(1, "--> [STATUS]: DEADLOCK DETECTED! Deadlocked processes: ");
    for(int i = 0; i < deadlocked_count; i++) {
      printf(1, "P%d ", deadlocked[i]);
    }
    printf(1, "\n");
    return 1;
  } else {
    printf(1, "--> [STATUS]: NO DEADLOCK DETECTED. System is in a safe/valid state.\n");
    return 0;
  }
}

// ==========================================
// Task (d): Banker's Algorithm (Avoidance)
// ==========================================
int is_safe_state(int Allocation[MAX_P][MAX_R], int Max[MAX_P][MAX_R], int Available[MAX_R]) {
  int Need[MAX_P][MAX_R];
  int Work[MAX_R];
  int Finish[MAX_P];
  int safeSeq[MAX_P];

  for(int i = 0; i < N; i++) {
    Finish[i] = 0;
    for(int j = 0; j < M; j++) {
      Need[i][j] = Max[i][j] - Allocation[i][j];
    }
  }

  for(int j = 0; j < M; j++) Work[j] = Available[j];

  int count = 0;
  while(count < N) {
    int found = 0;
    for(int i = 0; i < N; i++) {
      if(!Finish[i]) {
        int can_execute = 1;
        for(int j = 0; j < M; j++) {
          if(Need[i][j] > Work[j]) {
            can_execute = 0;
            break;
          }
        }

        if(can_execute) {
          for(int j = 0; j < M; j++) Work[j] += Allocation[i][j];
          safeSeq[count++] = i;
          Finish[i] = 1;
          found = 1;
        }
      }
    }
    if(!found) break;
  }

  if(count == N) {
    printf(1, "--> [BANKER]: STATE IS SAFE. Safe Execution Sequence: ");
    for(int i = 0; i < N; i++) printf(1, "P%d ", safeSeq[i]);
    printf(1, "\n");
    return 1;
  } else {
    printf(1, "--> [BANKER]: STATE IS UNSAFE! Granting this request leads to deadlock.\n");
    return 0;
  }
}

// ==========================================
// Task (c): Pipe-based Circular Wait Simulation
// ==========================================
void simulate_pipe_deadlock() {
  printf(1, "\n=======================================================\n");
  printf(1, "   CIRCULAR WAIT SIMULATION (Real xv6 Processes & Pipes)\n");
  printf(1, "=======================================================\n");

  int pipe1[2], pipe2[2];
  if(pipe(pipe1) < 0 || pipe(pipe2) < 0) {
    printf(1, "Pipe creation failed\n");
    exit();
  }

  printf(1, "Created Pipe1 (R:%d, W:%d) and Pipe2 (R:%d, W:%d)\n",
         pipe1[0], pipe1[1], pipe2[0], pipe2[1]);

  int pid = fork();

  if(pid == 0) {
    // Child (Process B / P1)
    // Process B holds Pipe2 Write end, waits to read from Pipe1
    printf(1, "Process B (PID %d): Holds Pipe2 Write (%d), waiting to Read Pipe1 (%d)...\n", 
           getpid(), pipe2[1], pipe1[0]);
    
    char buf;
    read(pipe1[0], &buf, 1); // Blocks forever waiting for Process A
    printf(1, "Process B completed\n");
    exit();
  } else {
    // Parent (Process A / P0)
    // Process A holds Pipe1 Write end, waits to read from Pipe2
    printf(1, "Process A (PID %d): Holds Pipe1 Write (%d), waiting to Read Pipe2 (%d)...\n", 
           getpid(), pipe1[1], pipe2[0]);

    // Construct resource matrix using standard MAX_P (5) x MAX_R (3) size:
    // P0 holds Pipe1 (R0), requests Pipe2 (R1)
    // P1 holds Pipe2 (R1), requests Pipe1 (R0)
    int PipeAlloc[MAX_P][MAX_R] = {
      {1, 0, 0},
      {0, 1, 0},
      {0, 0, 0},
      {0, 0, 0},
      {0, 0, 0}
    };

    int PipeReq[MAX_P][MAX_R] = {
      {0, 1, 0},
      {1, 0, 0},
      {0, 0, 0},
      {0, 0, 0},
      {0, 0, 0}
    };

    int PipeAvail[MAX_R] = {0, 0, 0};

    printf(1, "\nResource Dependency Matrix for Pipe Deadlock:\n");
    printf(1, "P0 (Proc A) -> Holds Pipe1, Waits Pipe2\n");
    printf(1, "P1 (Proc B) -> Holds Pipe2, Waits Pipe1\n");

    // Execute deadlock detection on 2 processes
    int temp_N = N;
    N = 2;
    detect_deadlock(PipeAlloc, PipeReq, PipeAvail);
    N = temp_N;

    // Clean up child process
    kill(pid);
    wait();
    close(pipe1[0]); close(pipe1[1]);
    close(pipe2[0]); close(pipe2[1]);
  }
}

// ==========================================
// Main Function — Test Cases Execution
// ==========================================
int main(int argc, char *argv[]) {
  printf(1, "\n=======================================================\n");
  printf(1, "       TEST CASE 1: Deadlock Detection (Safe State)\n");
  printf(1, "=======================================================\n");

  int Available1[3] = {2, 3, 0};
  int Allocation1[5][3] = {
    {0, 1, 0},
    {2, 0, 0},
    {3, 0, 3},
    {2, 1, 1},
    {0, 0, 2}
  };
  int Request1[5][3] = {
    {0, 0, 0},
    {2, 0, 2},
    {0, 0, 0},
    {1, 0, 0},
    {0, 0, 2}
  };

  print_matrices(Allocation1, Request1, Available1);
  detect_deadlock(Allocation1, Request1, Available1);

  printf(1, "\n=======================================================\n");
  printf(1, "      TEST CASE 2: Deadlock Detection (Deadlocked State)\n");
  printf(1, "=======================================================\n");

  int Available2[3] = {0, 0, 0};
  int Allocation2[5][3] = {
    {0, 1, 0},
    {2, 0, 0},
    {3, 0, 3},
    {2, 1, 1},
    {0, 0, 2}
  };
  int Request2[5][3] = {
    {0, 0, 0},
    {2, 0, 2},
    {0, 0, 1}, // P2 now requests R2 (causes circular dependency)
    {1, 0, 0},
    {0, 0, 2}
  };

  print_matrices(Allocation2, Request2, Available2);
  detect_deadlock(Allocation2, Request2, Available2);

  printf(1, "\n=======================================================\n");
  printf(1, "   TEST CASE 3: Banker's Algorithm (Request Evaluation)\n");
  printf(1, "=======================================================\n");

  int Available3[3] = {3, 3, 2};
  int Allocation3[5][3] = {
    {0, 1, 0},
    {2, 0, 0},
    {3, 0, 2},
    {2, 1, 1},
    {0, 0, 2}
  };
  int Max3[5][3] = {
    {7, 5, 3},
    {3, 2, 2},
    {9, 0, 2},
    {2, 2, 2},
    {4, 3, 3}
  };

  printf(1, "[Initial State Check]:");
  print_matrices(Allocation3, Max3, Available3);
  is_safe_state(Allocation3, Max3, Available3);

  printf(1, "\nEvaluating Hypothetical Request: Process P1 requests (1, 0, 2)\n");
  // Apply hypothetical allocation for P1
  Available3[0] -= 1; Available3[1] -= 0; Available3[2] -= 2;
  Allocation3[1][0] += 1; Allocation3[1][1] += 0; Allocation3[1][2] += 2;

  is_safe_state(Allocation3, Max3, Available3);

  // Run Circular Wait simulation
  simulate_pipe_deadlock();

  exit();
}