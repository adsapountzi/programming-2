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
    struct stat buf;
    char file_name[250], short_name[SIZE_NAME], file_array[SIZE_MAX], *ptr;
    int size_of_file_name, result, fd1, i;
    off_t size_of_file;
    time_t atime, mtime;
    
    while(scanf(" %s", file_name) == 1) {    /*reads the regular files.*/
        
        for(i = 0; i < SIZE_NAME; i++) {
            short_name[i] = '\0';
        }
        
        ptr = strrchr(file_name, '/');           /*keeps the name without the path*/
        if( ptr == NULL) {
            strcpy(short_name, file_name);     
        }
        else{
            strcpy(short_name, ptr + 1);
        }
        size_of_file_name = strlen(short_name);
        
        stat(file_name, &buf);         /*receive the information of the file before the opening*/
        atime = buf.st_atime;
        mtime = buf.st_mtime;
//         fprintf(stderr, "oldarchive before open atmine %lu modtime %lu\n", atime, mtime);
        fd1 = open(file_name, O_RDWR, S_IRWXU);                 /*opens the file and writes the size of its name , the name , the last access and modification, the access rights and the type of the file, its size and  contents.*/
        examine(fd1, __LINE__);

        my_write(STDOUT_FILENO, &size_of_file_name, sizeof(int), __LINE__);

        my_write(STDOUT_FILENO, short_name, size_of_file_name, __LINE__);
//               fprintf(stderr, "oldarchive after open atmine %lu modtime %lu\n", atime, mtime);

        my_write(STDOUT_FILENO, &atime, sizeof(time_t), __LINE__);
        
        my_write(STDOUT_FILENO, &mtime, sizeof(time_t), __LINE__);
        my_write(STDOUT_FILENO, &buf.st_mode, sizeof(mode_t), __LINE__);

        size_of_file = buf.st_size;

        my_write(STDOUT_FILENO, &size_of_file, sizeof(off_t), __LINE__);

        while(size_of_file > SIZE_MAX) {            /*Transfering the contents of the file in blocks of maximum 512 bytes*/
                my_read(fd1, file_array, SIZE_MAX, __LINE__);
                my_write(STDOUT_FILENO, file_array, SIZE_MAX, __LINE__);
                
                size_of_file -= 512;

        }

        if(size_of_file > 0) {
            my_read(fd1, file_array, size_of_file,__LINE__ );
            my_write(STDOUT_FILENO, file_array, size_of_file, __LINE__);
        }
        
        result = close(fd1);
        examine(result, __LINE__);
        
    }
    return(0);
}
