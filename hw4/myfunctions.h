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

struct info {
    pid_t pid;
    char *name, **arguements;
    int activity, argc;
    struct info *next;
    struct info *prev;
};
struct info *head;
struct info *find_proccess(pid_t pid);
void examine_mem_allocation( void *ptr);
void examine(int result,int line);
ssize_t my_read(int fd, char *buf, size_t size, int line);
void init_list();
void insert_node(pid_t pid, char name[], char *arguements[], int num_of_args);
void print_list();
void delete_node(pid_t pid);
void delete_all();
