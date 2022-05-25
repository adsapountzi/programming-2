#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include "myfunctions.h"


static volatile sig_atomic_t gotsig = -1;

static void handler(int sig) {
    gotsig = sig;
    printf("Received signal SIGUSR1!\n");
}

int main(int argc, char *argv[]) {
    int i, max_reps, b, result;
    pid_t pid;
    struct sigaction act = {{0}};
    sigset_t s1;
    
    pid = getpid();
    
    b = atoi(argv[4]);
    
    max_reps = atoi(argv[2]);
    
    act.sa_handler = handler;
        
    result = sigaction(SIGUSR1, &act, NULL); 
    examine(result, __LINE__);

    result = sigemptyset(&s1);
    examine(result, __LINE__);
    
    result = sigaddset(&s1 ,SIGUSR1);
    examine(result, __LINE__);
    
    for(i = 0; i < max_reps; i++) {
        if(b == 1) {
            if(i == 0) {
                result = sigprocmask(SIG_BLOCK, &s1, NULL);
                examine(result, __LINE__);    
            }
            else if(i == ((max_reps / 2) + 1)) { 
                result = sigprocmask(SIG_UNBLOCK, &s1, NULL);
                examine(result, __LINE__);
            }
        }
        if(gotsig != -1) {
            i = 0;
            gotsig = -1;
        }
        
        printf(" [%s] %d %d/%d\n",argv[0], pid, i, max_reps);
        sleep(5);    
    }
    return(0);
}