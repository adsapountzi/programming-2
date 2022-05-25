#include "myfunctions.h"

struct info *find_proccess(pid_t pid) {
    struct info *current;
    
    for(current = head->next;  (current->pid != pid && current != head); current = current->next) {
                 fprintf(stderr, "p %d curr %d\n", pid, current->pid);
    }
    return(current);
}

void examine_mem_allocation( void *ptr) {
    if(ptr == NULL) {
        fprintf(stderr,"Malloc failed! %d\n", __LINE__);
       
        exit(1);  
    }
}
void examine(int result,int line) {
    if(result == -1) {
    fprintf(stderr, "Error line: %d\n", line);
    perror("PER");
    exit(0);
    }
}

ssize_t my_read(int fd, char *buf, size_t size, int line) {
    ssize_t bytes = 0, num = 0;
    size_t result = 0;
    

    do{ 
        bytes = read(fd, buf + num, size);
//          fprintf(stderr, "bytes read un %ld\n", bytes);
        if(bytes == -1) {
            return(bytes);
        }
        num += bytes;

        result = (size_t)bytes;
        size -= result;
    }
    while(buf[num -1] != '\n');
    
    return(num);
}

void init_list() {
    head = (struct info *)malloc(sizeof(struct info));
    if(head == NULL) {
        fprintf(stderr,"Malloc failed! %d\n", __LINE__);
        exit(1);
    }
    head->next = head;
    head->prev = head;
}

void insert_node(pid_t pid, char name[], char *arguements[], int num_of_args) {
    struct info *node;
    int i;
    
    node = (struct info *)malloc(sizeof(struct info));
    if(node == NULL) {
        fprintf(stderr,"Malloc failed! %d\n", __LINE__);
        exit(1);   

    }
    node->arguements = (char **)malloc(num_of_args * sizeof(char *));
    if(node->arguements == NULL) {
        fprintf(stderr,"Malloc failed! %d\n", __LINE__);
        exit(1);    
    }
    
    node->argc = num_of_args;
    node->name = strdup(name);

    if(name == NULL) {
        fprintf(stderr,"Malloc failed! %d\n", __LINE__);
        exit(1);    
    }
    
    for(i = 0; i < node->argc; i++) {
        node->arguements[i] = (char *)malloc(sizeof(arguements[i]));
        if(node->arguements[i] == NULL) {
            fprintf(stderr,"Malloc failed! %d\n", __LINE__);
            exit(1);    
        }
        strcpy(node->arguements[i], arguements[i]);
    }
    
    node->activity = 0;
    
    node->pid = pid;
    node->prev = head->prev;
    node->next = head;
    head->prev->next = node;
    head->prev = node;
}

void print_list() {
    struct info *current;
    int i;
    
    printf("list:\n");
    for(current = head->next; current != head; current = current->next) {
        if(current->activity != -1) {
            printf("pid: %d, name: (", current->pid);
            
            for(i = 0; i < current->argc; i++) {
                printf(" %s", current->arguements[i]);
            }
            
            printf(" )");
            
            if(current->activity == 1) {
                printf(" (R)");
            }
            printf("\n");
        }
    }
}

void delete_node(pid_t pid) {
    struct info *current;
    int i;
    
    for(current = head->next; current->pid != pid; current = current->next);
        current->prev->next = current->next;
        current->next->prev = current->prev;
        free(current->name);
        for(i = 0; i < current->argc; i++) {
            free(current->arguements[i]);
        }
        free(current->arguements);
            
        free(current);
}

void delete_all() {
    struct info *current, *next_node;
    int i, active_proccesses = 0;
    pid_t result;
    struct sigaction act = { {0} };
    
    act.sa_handler = SIG_DFL;
     
    sigaction(SIGCHLD, &act, NULL);
     
    for(current = head->next; current != head; current = next_node) {
        next_node = current->next;
        active_proccesses++;
        examine(kill((current->pid), SIGKILL), __LINE__); 
        
//          current->prev->next = current->next;
//          current->next->prev = current->prev;
        
        free(current->name);
        
        for(i = 0; i < current->argc; i++) {
            free(current->arguements[i]);
        }
        free(current->arguements);
            
        free(current);
    }
    for(i = 0; i < active_proccesses; i++) {
        result = waitpid(-1, NULL, 0);
        if(result == -1) {
            perror("fail line");
            exit(1);
        }
    }
    
    free(head);
}