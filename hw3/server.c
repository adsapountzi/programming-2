/*A program that makes a booking system for airline tickets that is used by the server and receives the updated reservations and about available seats of each flight.
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
#include <sys/select.h>
#include <sys/sem.h>
#include "myfunctions.h"

#define SIZE 300

struct flight_info {
    char airline[3];
    char src[4];
    char dest[4];
    int intermediate_flights;
    int seats;
};

struct status{
    pid_t pid;
    int fd;
    int flights_reserved;
};

int main(int argc, char* argv[]) {
    key_t shmem_key;
    int n, shmid, lines = 0, res, res1, i = 0, client_num = 0, c, max_client_number, num_of_res_seats, f[3], max_fd = 0, fd1, fd0, j, fd, semid, result, k = 0, input_bytes = 1;
    char fifo_name1[100], fifo_name2[100], server_input;
    struct status status[SIZE];
    struct flight_info info, *p, *p1;
    FILE *fp;
    fd_set fds1, fds2;
    
    for(j = 0; j < SIZE; j++) {
        status[j].flights_reserved = 0;
        status[j].pid = -1;
    }

    max_client_number = atoi(argv[1]); 
    
    shmem_key  = ftok(argv[2], 1);
    if(shmem_key == -1) {
        fprintf(stderr, "Wrong shared memory key LINE %d\n", __LINE__);
        exit(1);
    }
    
    if((fp = fopen( argv[2], "r+")) == NULL) {
        fprintf(stderr, "fopen failed!!\n");
        return(0);
    }
    
    for(c = fgetc(fp); c != EOF; c = fgetc(fp)) {
        if( c == '\n'){
            lines +=1;
            
        }
    }
    
    shmid = shmget(shmem_key, (size_t)(lines * sizeof(struct flight_info)), IPC_CREAT | S_IRWXU );  /*create shared memory.*/
    if( shmid == -1) {
        fprintf(stderr, "Error shmid! LINE %d\n", __LINE__); 
    }
    
    semid = semget(IPC_PRIVATE, 1, IPC_CREAT | S_IRWXU );
    examine(semid, __LINE__);
    
    semctl(semid, 0, SETVAL, 1);    
    
    p = (struct flight_info *)shmat(shmid, NULL, 0);
    
    p1 = p;
    res = fseek(fp, (long)0, SEEK_SET);
    examine(res, __LINE__);
    
    while(fscanf(fp, "%s %s %s %d %d", info.airline, info.src, info.dest, &info.intermediate_flights, &info.seats) == 5) {             /*receive file info*/
        memcpy(p1, &info, sizeof(struct flight_info));

        p1 += 1;
    }
    
    f[0] = mkfifo(argv[3],S_IRWXU);
    examine(f[0], __LINE__);
   
    fd0 = open(argv[3], O_RDONLY);
    examine(fd0, __LINE__);
    
    if( max_fd < fd0) {
        max_fd = fd0;
    }
   
    FD_ZERO(&fds1);
    FD_SET(fd0, &fds1);
    FD_SET(STDIN_FILENO, &fds1);

    while(1){
        fds2 = fds1;

        n = select(max_fd + 1, &fds2, NULL, NULL, NULL);         /*examine which fd is ready.*/
        examine(n, __LINE__);
	if(n == 0) {
	    	    fprintf(stderr, "select n = 0\n");	    
	}
        if(n > 0) {
            for(fd = 0; fd <= max_fd; fd++) {
                if(FD_ISSET(fd, &fds2)){
                    if(fd == fd0) {                        /*If the server receives a request from the agent.*/
                        for(k = 0; status[k].pid != -1; k++);
                        
                        result = my_read(fd0, &(status[k].pid), sizeof(pid_t), __LINE__);
                        if(result == 0) {
                           break;
                        }
                        
                        snprintf(fifo_name1, sizeof(fifo_name1), "fifo_1_%ld", (long)(status[k].pid));
                        
                        fd1 = open(fifo_name1, O_WRONLY);
                        examine(fd1, __LINE__);
                        
                        if(client_num + 1 > max_client_number) {        /*examine if the agents have reached the maximum number*/
                            fprintf(stderr,"Reached max client number!!\n");
                            examine(close(fd1), __LINE__);
                        }
                        else{
                            client_num++;                    
                            printf("Agent pid %d\n", status[k].pid);
                            snprintf(fifo_name2, sizeof(fifo_name2), "fifo%ld", (long)(status[k].pid));
                            
                            f[2] = mkfifo(fifo_name2, S_IRWXU);     /*creates a pipe in order to tranfer the info about reservations*/
                            examine(f[2], __LINE__);
                            
                            my_write(fd1, &shmid, sizeof(int), __LINE__);      /*the server sents the information needed in order to access the shared memory*/
                            my_write(fd1, &lines, sizeof(int), __LINE__);
                            my_write(fd1, &semid, sizeof(int), __LINE__);
                                                    
                            examine(close(fd1), __LINE__);
                           
                            status[k].fd = open(fifo_name2, O_RDONLY);
                            examine(status[k].fd, __LINE__);
                            
                            FD_SET(status[k].fd, &fds1);       
                            if( max_fd < status[k].fd) {
                            max_fd = status[k].fd;
                            }
                            
                            
                            
                        }
                    }
                    if((fd != STDIN_FILENO) && (fd != fd0)) { /*if the agent makes a reservation*/
                        for(i = 0; fd != status[i].fd; i++);

                        res1 = my_read(status[i].fd, &num_of_res_seats, sizeof(int), __LINE__);    
                        
                        examine(res1, __LINE__); 
                        if(res1 > 0 ) {                            
                            if(num_of_res_seats == -1) {       /*if the agent exits*/
                                FD_CLR(status[i].fd, &fds1);
                                printf("Agent %ld reserved %d total seats!\n", (long)(status[i].pid), status[i].flights_reserved);
                                status[i].pid = -1;
                                status[i].flights_reserved = 0;
                                client_num--;
                                                                
                                examine(close(status[i].fd), __LINE__);
                                if(client_num == 0) {       
                                    break;
                                    
                                }
                                if(status[i].fd == max_fd) {
                                    max_fd--;
                                }
                            }
                                
                            if(num_of_res_seats > 0 && (status[i].pid != -1) ){
                                printf("Agent %ld reserved %d seats!\n", (long)(status[i].pid), num_of_res_seats);
                                status[i].flights_reserved += num_of_res_seats;
                            }
                        }
                    }
                    if(fd == STDIN_FILENO) {      
                        input_bytes = my_read(STDIN_FILENO, &server_input,sizeof(char), __LINE__);                                                  
                        if(server_input == 'Q' || (input_bytes == 0)) {
                             break;
                       }
                    }
                }
            }
        }
        
        if(client_num == 0) {
            break;
        }
        if(server_input == 'Q' || (input_bytes == 0)|| (result == 0)) {
            break;
        }
    }
    
                
    for(j = 0; j < SIZE; j++) {
        if(status[j].pid != -1) {
            printf("Agent %ld reserved %d seats in total!\n", (long)(status[j].pid), status[j].flights_reserved);
        }
    }
    res = fseek(fp, (long)0, SEEK_SET);
    examine(res, __LINE__);
    p1 = p;
    
    for(j = 0; j < lines; j++) {
        fprintf(fp, "%2s %3s %3s %d %d\n", p1->airline, p1->src, p1->dest, p1->intermediate_flights,p1->seats);
        p1 += 1;
    }
    examine(close(fd0), __LINE__);
    
    unlink(argv[3]);

    res = fclose(fp);
    if(res == EOF) {
        printf("fclose failed\n");
        return(0);
    }
    
    semctl(semid,0,IPC_RMID);
    shmdt(p);
    shmctl(shmid, IPC_RMID, NULL);

 return(0);
}
