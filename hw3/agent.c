/*A program that makes a booking system for airline tickets that is used by the agents.Each agent sends a request and can have access to shared memory in which can make reservations.
 *
 * Authors: A. Sapountzi aem: 02624
 * Date: 6/5/2018
*/
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "myfunctions.h"
#define SIZE 30

struct flight_info {
    char airline[3];
    char src[4];
    char dest[4];
    int intermediate_flights;
    int seats;
};

int main(int argc, char* argv[]) {
    int fd1, fd2, fd3, f1, shmid, res_code, lines_read = 0, lines, res_dest, res_src, j = 0, i, client = -1, semid;
    pid_t client_pid;
    struct flight_info *p, *p1, info;
    char fifo_name1[100], option[100], fifo_name2[100];
    struct sembuf op;
    
    client_pid = getpid();
    snprintf(fifo_name1, sizeof(fifo_name1), "fifo_1_%ld", (long)client_pid);
    snprintf(fifo_name2, sizeof(fifo_name2), "fifo%ld", (long)client_pid);
    
    f1 = mkfifo(fifo_name1,S_IRWXU);        /*creates a pipe in order to receive the info in order to access the shared memory*/
    examine(f1, __LINE__);
    
    fd1 = open(argv[1], O_WRONLY);
    examine(fd1, __LINE__);
    
    my_write(fd1, &client_pid, sizeof(pid_t), __LINE__);
    
    fd2 = open(fifo_name1, O_RDONLY);
    examine(fd2, __LINE__);
    
    if(my_read(fd2, &shmid, sizeof(int), __LINE__) == 0) {
        return(0);
    }

    my_read(fd2, &lines, sizeof(int), __LINE__);
        
    my_read(fd2, &semid, sizeof(int), __LINE__);
    
    p = (struct flight_info *)shmat(shmid, NULL, 0);
    fd3 = open(fifo_name2, O_WRONLY);
    examine(fd3, __LINE__);

    while(1) {
        for(j = 0; j < 100; j++) {
            option[j] = '\0';
        }

        printf("Select option\n");
        scanf(" %s", option);
       
        if(!(strcmp(option, "EXIT"))) {
            my_write(fd3, &client, sizeof(int), __LINE__); 
            break;
        }
        if(!(strcmp(option, "FIND"))) {
            op.sem_num = 0;
            op.sem_op = -1;
            op.sem_flg = 0;
            semop(semid, &op, 1);
             
            p1 = p;
            lines_read = 0;
            printf("FIND SRC DEST NUM\n");        
            scanf(" %s %s %d", info.src, info.dest, &info.seats);
            i = 0;
           
            while(1) {
                res_dest = strcmp((p1->dest), info.dest);
                res_src = strcmp((p1->src), info.src);
                
                if(res_dest == 0 && res_src == 0 && (info.seats <= (p1->seats))) {
                    printf("%s %s %s %d %d\n", p1->airline, p1->src, p1->dest, p1->intermediate_flights, p1->seats); 
                    i++;
                }
                if (lines == lines_read) {
                    if(i == 0){
                    fprintf(stderr,"Research not found!\n");
                    }
                    break;
                }
                
                p1 += 1;
                lines_read += 1;
            }
            op.sem_num = 0;
            op.sem_op = 1;
            op.sem_flg = 0;
            semop(semid, &op, 1);           
        }
        
        if(!(strcmp(option, "RESERVE"))) {
            op.sem_num = 0;
            op.sem_op = -1;
            op.sem_flg = 0;
            semop(semid, &op, 1);
            
            p1 = p;
            lines_read = 0;
            printf("RESERVE SRC DEST AIRLINE NUM\n");
            
            scanf(" %s %s %s %d", info.src, info.dest, info.airline, &info.seats);
        
            while(1) {
                res_code = strcmp((p1->airline), info.airline);
                res_dest = strcmp((p1->dest), info.dest);
                res_src = strcmp((p1->src), info.src);
              
                if( (res_code == 0 && res_dest == 0) && res_src == 0) {
                    if( info.seats <= (p1->seats)){
                        p1->seats -= info.seats;
                
                        my_write(fd3, &info.seats, sizeof(int), __LINE__); 
                        
                        printf("Seats successfully reserved!\n");
                        break;
                    }
                }
                if (lines == lines_read) {
                    info.seats = 0;
                    fprintf(stderr, "Reservation failed!\n");
                    my_write(fd3, &info.seats, sizeof(int), __LINE__); 

                    break;
                }
                p1 += 1;
                lines_read += 1;
            }
            op.sem_num = 0;
            op.sem_op = 1;
            op.sem_flg = 0;
            semop(semid, &op, 1);           
        }
    }
    examine(close(fd3), __LINE__);
    examine(close(fd1), __LINE__);
    examine(close(fd2), __LINE__);
    unlink(fifo_name1);
    unlink(fifo_name2);

    shmdt(p);
    
    return(0);
}
