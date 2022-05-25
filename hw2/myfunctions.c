#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<time.h>
#include "myfunctions.h"

ssize_t my_read(int fd, void *buf, size_t size, int line) {
    ssize_t bytes = 0, num = 0;
    size_t result = 0;
    

    do{ 
        
        bytes = read(fd, buf + num, size);
//         fprintf(stderr, "bytes read un %ld\n", bytes);
        if( bytes == -1) {
	    fprintf(stderr, "Error line: %d\n", line);
	    exit(0);
	}
        
        num += bytes;
//         fprintf(stderr, " num un r %ld\n", num);
//      printf(" read %d %d %d\n", result, size, i);
        result = (size_t)bytes;
        size -= result;
    }
    while(bytes != 0);
    
    return(num);
}

void my_write(int fd, void *buf, size_t size, int line) {
    ssize_t bytes = 0, num = 0;
    size_t result = 0;
    
    do{ 
        bytes = write(fd, buf + num, size);
//         fprintf(stderr, " bytes crypt w %ld\n", bytes);
        if( bytes == -1) {
	    fprintf(stderr, "Error line: %d\n", line);
	    exit(1);
	}        
//      printf(" write %d %d %d\n", result, size, i);
        num += bytes;
        result = (size_t)bytes;
        size -= result;
    }
    while(bytes != 0);
}

void examine(int result,int line) {
    if(result == -1) {
    fprintf(stderr, "Error line: %d %d\n", line, errno);
    exit(0);
    }
}