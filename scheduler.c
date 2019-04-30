#include <ucontext.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>
#include <getopt.h>
#include <string.h>
#include <math.h>
#include <time.h>


int main(int argc, char *argv[])
{

    // parsing command line arguments
    int i=1;
    int T[argc];
    int tottime = 0;
    for(i=1;i<argc;i++){
        T[i] = atoi(argv[i]);
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




return 0;
}
