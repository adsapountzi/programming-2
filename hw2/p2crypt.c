#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<time.h>
#include "myfunctions.h"
#define SIZE_MAX 512
#define SIZE_NAME 100
    
int main(int argc, char* argv[]) {
    int num_of_key, i = 0, k, j = 0;
    char key[SIZE_NAME], array[SIZE_MAX], new_array[SIZE_MAX];
    ssize_t bytes_read;
    
    strcpy(key, argv[1]);
    num_of_key = strlen(key);
    
    do {            /*Transfering the contents of the file in blocks of the num of key*/
        bytes_read = my_read(STDIN_FILENO, array, num_of_key, __LINE__);

        if(bytes_read != 0) {
            for(k = 0; k < bytes_read; k++) {
                new_array[j] = array[j] ^ key[i];  /*it crypts the contents*/
		j++;
                i++;
                if(i == num_of_key) {
                    i = 0;
                }
                if( j == bytes_read) {          
                    my_write(STDOUT_FILENO, new_array, bytes_read, __LINE__);
                    j = 0;
                }
            }
        }
        
    }
    while(bytes_read != 0);
    
    return(0);
}
    
