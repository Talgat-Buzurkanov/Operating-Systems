/*
    Talgat Buzurkanov
    METU EE442 Homework 2
    "User-level thread scheduling using Round-Robin approach with time quantum of 1 unit of time"

    Original code is borrowed from https://github.com/xanpeng/snippets/blob/master/coroutine/ucontext-example.c#L2

*/

#include <ucontext.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>
#include <string.h>
#include <time.h>

#define STACK_SIZE 1024
#define NUMCONTEXTS 6

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
  int p = 1;
 // printf("\t\t\t\t\t\t\t\t\t\t\tscheduling out Thread-%d and scheduling in Thread-%d\n",prev,current);
  for(p; p < current; p++ ){
      printf("\t\t");
  }
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

  int counter;
  counter = atoi((char*)arg);
  for(int t=1 ; t<=counter ; t++ ){
    printf("\t  %s \n", (char*)arg);
    scheduler();
  }
  
}




int main(int argc, char *argv[])
{

    // parsing command line arguments
    if (argc <= 6){
        int i=1;
        int T[argc];
        char* T_char[argc];
        int tottime = 0;
        for(i=1;i<argc;i++){
            T[i] = atoi(argv[i]);
            T_char[i] = argv[i];
            tottime += T[i]; 
        };

        printf("\nshare:\n");

        int j = 1;
        for(j=1;j<argc;j++){
            
            printf("\t %d/%d \t",T[j],tottime);
        };

        printf("\n\nthreads:\n");

        int k = 1;
        for(k=1;k<argc;k++){
            
            printf("\t  T%d \t",k);
        };

        printf("\n");



        // createThread(runThread);  // Create a child context, but still have not triggered the call
        // createThread(runThread);
    createThread(runThread,T_char[1]);
    createThread(runThread,T_char[2]);
    createThread(runThread,T_char[3]);
    createThread(runThread,T_char[4]);
    createThread(runThread,T_char[5]);
        // for (int m=1; m<argc+1; m++){
        //         
        //         createThread(runThread,T_char[m]);
        //         //scheduler();
        //     }

        scheduler();

 
    




    }






    else {printf("Error.\nDesign requirement: Number of threads cannot exceed 5\n");}
    


return 0;
}
