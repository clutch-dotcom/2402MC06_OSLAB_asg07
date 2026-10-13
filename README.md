## Q1.
Implemented user level threads that share the same page table as the parent, but with its unique kernel stack by creating two functions int clone(void(*fcn)(void*), void *arg, void *stack) that creates a new thread sharing the same address space (pgdir) as the calling process, but with its own kernel stack, and begins executing fcn(arg) on the user-provided stack; and int join(void) system call that waits for a thread created by clone() to finish and cleans up its resources. 
To implement clone(..), call allocproc() inside the the clone, set the new process as a thread, copy the page directory and cwd and open files, copy the user provided stack into the new process's trap frame stack pointer(esp), set the instruction pointer in the trap frame to fcn.
To implement join(), it is done similarly to wait() i.e, it reaps zombie processes but if it is only marked as a thread, and it also does NOT free the page directory.
In Threadtest without thread sync, race conditions occur because incrementing a counter is not an atomic operation in assembly. So, when a process tries to increment the counter, they might read or write an obsolete or outdated value of the counter. This leads to the counter value being less than the expected 300000

## Q2.
Implemented thread synchronisation using a semaphore(implemented in the previous assignments) for the operation of incrementing the counter, which provides security for the operation of reading and writing the counter value and eliminating race conditions. 
Also implemented producer consumer problem using threads to clone multiple producers and consumers, and semaphores counting semaphores empty(=5), full(=0), and a mutex for mutual exclusion on the shared buffer.

## Q3.
