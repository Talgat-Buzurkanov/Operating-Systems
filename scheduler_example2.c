// gcc ucontext-example.c -o ucontext-example --std=gnu99
#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>
#include <signal.h>

#define STACK_SIZE 1024
#define NUMCONTEXTS 8

int current = 0;  // current context

typedef struct ThreadInfo {
  int state;
  ucontext_t context;
  char stack[STACK_SIZE];
  void* (*func)(void *arg);
  void *arg;
  void *exit_status;
} ThreadInfo;

static ThreadInfo threads_array[NUMCONTEXTS];  // All the contexts used to maintain the current thread/process

// from slots scheduling a context execution
void scheduler(void) {
  int i;
  for (i = (current + 1) % NUMCONTEXTS; i != current; i = (i + 1) % NUMCONTEXTS)            // round-robin
    if (threads_array[i].state) break;  // Find the first active context behind the current in the slots

  int prev = current;
  current = i;
  printf("scheduler stop coro-%d, and running coro-%d\n", prev, current);
  swapcontext(&threads_array[prev].context, &threads_array[current].context);  // Save the old state and switch to the new state
}

void exitThread(void) {
  void *exit_status = threads_array[current].func(threads_array[current].arg);
  threads_array[current].exit_status = exit_status;
  threads_array[current].state = 0;
  scheduler();
}

int createThread(void* (*start_routine)(void*), void *arg) {
  static int last_used = 0;
  int i, tid;

  for (i = (last_used + 1) % NUMCONTEXTS; i != last_used; i = (i + 1) % NUMCONTEXTS)
    if (!threads_array[i].state) break;
  if (i == last_used)
    return -1;
  last_used = i;
  tid = i;

  getcontext(&threads_array[tid].context);  // Save the current, active context to ucp
  threads_array[tid].context.uc_stack.ss_sp = threads_array[tid].stack;  // You must set uc_stack, and uc_link before calling makecontext.
  threads_array[tid].context.uc_stack.ss_size = sizeof(threads_array[tid].stack);
  threads_array[tid].context.uc_link = &threads_array[0].context;

  threads_array[i].state = 1;
  threads_array[i].func = start_routine;
  threads_array[i].arg = arg;
  threads_array[i].exit_status = 0;
  // 原型：makecontext(ucontext_t* ucp, void (*func)(), int argc, ...);
  makecontext(&threads_array[i].context, exitThread, 0);  // Modify the ucp obtained by getcontext. If ucp is activated by setcontext/swapcontext, the function func will be called.
  return 0;
}

void* runThread(void *arg) {
  printf("running coro-%d runThread(%s)\n", current, (char*)arg);
  scheduler();
}

int main(int argc, char *argv[]) {
  createThread(runThread, "coro1 args");  // Create a child context, but still have not triggered the call
  createThread(runThread, "coro2 args");
  createThread(runThread, "coro2 args");

  scheduler();

  return 0;
}







/*
 * scheduler stop coro-0, and running coro-1
 * running coro-1 runThread(coro1 args)
 * scheduler stop coro-1, and running coro-2
 * running coro-2 runThread(coro2 args)
 * scheduler stop coro-2, and running coro-2
 * scheduler stop coro-2, and running coro-2
*/
