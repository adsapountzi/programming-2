/*A program that implements an environment of executing programs that supports some commands.
 * Authors: A. Sapountzi aem: 02624
 * Date: 27/5/2018*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/time.h>
#include <errno.h>
#include "myfunctions.h"

#define MAX_SIZE 512
#define SIZE 100
#define MAX_ARGS 50

volatile sig_atomic_t got_sigchld = -1, got_sigusr1 = -1, got_sigalrm = -1;

static void handle_exit(int sig) {
    got_sigchld = 1;
}

static void handler(int sig) {
    got_sigusr1 = 1;
}

static void handle_alarm(int sig) {
    got_sigalrm = 1;
}

int main(int argc, char *argv[]) {
    char *exec_argv[MAX_ARGS], word[MAX_SIZE], *command, *token;
    pid_t pid, sig_pid, term_pid;
    int num_of_args, i, res, k;
    struct sigaction act1 = { {0} }, act2 = { {0} }, act_alarm = { {0} };
    struct info *current, *next_node;
    struct itimerval t = { {0} };
    ssize_t bytes;
    
    act1.sa_handler = handle_exit;

    act2.sa_handler = handler;
    
    act_alarm.sa_handler = handle_alarm;

    sigaction(SIGCHLD, &act1, NULL);
    sigaction(SIGUSR1, &act2, NULL);
    sigaction(SIGALRM, &act_alarm, NULL);
    
    
    t.it_value.tv_sec = 20;
    t.it_value.tv_usec = 0;
    t.it_interval.tv_sec = 20;
    t.it_interval.tv_usec=0;
    
    setitimer(ITIMER_REAL, &t, NULL);
    
    init_list(); /* initialization of the list*/
    
    while(1) { 
        printf("Select command!\n");
        if(got_sigchld == 1) {
            while(1) {
                pid = waitpid(-1, NULL, WNOHANG);
                if(pid == 0) {
                    break;
                }
                if(pid == -1) {
                   perror("waitpid"); 
                   break;
                }
                printf("Proccess exited %d\n", pid);
           /* in case the proccess has exited.*/
                current = find_proccess(pid);
                next_node = current->next;
                if( (!(current->next == head && current->prev == head))) {
                    if(next_node != head) {
                        examine(kill(next_node->pid, SIGCONT), __LINE__);
                        next_node->activity = 1;
                    }
                    if(next_node == head && head->next != head) {
                        next_node = head->next;
                        examine(kill(next_node->pid, SIGCONT), __LINE__);
                        next_node->activity = 1;
                    }
                }
                delete_node(pid);
            }
            
            got_sigchld = -1;
        }
        if(got_sigusr1 == 1) {  /* In case SIGUSR1 is called the main program promotes the signal to all existing proccess. */
            for(current = head->next; current != head; current = current->next) {
                examine(kill(current->pid, SIGUSR1), __LINE__); 
            }
            got_sigusr1 = -1;
        }
        
        if(got_sigalrm == 1) { 
            for(current = head->next; current != head; current = current->next) {
                if(current->activity == 1 && (!(current->next == head && current->prev == head))) {
                    examine(kill(current->pid, SIGSTOP), __LINE__);
                    current->activity = 0;
                    next_node = current->next;
                    
                    if((next_node == head) && (head->next != head) && (head->next != current)) {
                        current = head->next;
                        examine(kill(current->pid, SIGCONT), __LINE__);
                        current->activity = 1;
                    }
                    else if(current->next != head) {
                        current = current->next;
                        examine(kill(current->pid, SIGCONT), __LINE__);
                        current->activity = 1;
                    }   
                }
            }
            printf("BEEP!\n");
            got_sigalrm = -1;
        }
        
        for(i = 0; i < MAX_SIZE; i++) {
            word[i] = '\0';
        }
        k = 0;
//         my_read(STDIN_FILENO,word, MAX_SIZE, __LINE__);
        bytes = my_read(STDIN_FILENO, word, MAX_SIZE, __LINE__);
        if( bytes == -1 && errno == EINTR) { /* In case a signal is called */
                continue;
            }
        if(bytes == -1) {
                fprintf(stderr, "Error line: %d\n", __LINE__);
            }
        token = strtok(word, " \n");
//             fprintf(stderr," TOKEN %s\n", token);
        /* walk through other tokens */
        
        while( token != NULL ) {
            if(k == 0) {
                 command = strdup(token);
            }
            
            if(k > 0){
                exec_argv[k -1] = strdup(token);
                if( exec_argv[k -1] == NULL) {
                    fprintf(stderr,"Malloc failed! %d\n", __LINE__);
                    exit(1);    
                }
            }
            k += 1;
            token = strtok(NULL, " \n");
            
        }
        /*examine the user's command*/
        if(!(strcmp(command, "exec"))) {
            num_of_args = k - 1;
            pid = fork();
            examine(pid, __LINE__);
            
            if(pid == 0) {
                exec_argv[k - 1] = NULL; 
                if(!strcmp("./firefox",exec_argv[0])) {
		    res = execv("/usr/bin/firefox", exec_argv);
		    examine(res, __LINE__); 
		}
                 res = execv(exec_argv[0], exec_argv);
                 examine(res, __LINE__); 
                
            }
            if(pid > 0) {
//                 fprintf(stderr, "pid > 0 %d\n", pid);
                insert_node(pid, exec_argv[0], exec_argv, num_of_args);
                for(current = head->next; current->pid != pid; current = current->next);
                if(!(current->next == head && current->prev == head)) {
                    examine(kill(current->pid, SIGSTOP), __LINE__);
                }
                else {
                    current->activity = 1;
                }
//                 current = find_proccess(pid);                
//                 fprintf(stderr, "pid >> 0\n");
            }
        }
        if(!(strcmp(command, "term"))) {
            term_pid = atoi(exec_argv[0]);
            examine(kill(term_pid, SIGTERM), __LINE__);   
           
        }
        
        if(!(strcmp(command, "sig"))) {
            sig_pid = atoi(exec_argv[0]);
            examine(kill(sig_pid, SIGUSR1), __LINE__);  
        }
        if(!(strcmp(command, "list"))) {
            print_list();
        }
         
        if(!(strcmp("quit" , command))) {      
            delete_all();
         
            break;
        }
        
        free(command);
        for(i = 0; i < k -1; i++) {
            free(exec_argv[i]);
        }
    }    
    
    return(0);
}    
