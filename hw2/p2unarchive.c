#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<time.h>
#include<utime.h>
#include "myfunctions.h"
#define SIZE_MAX 512
#define SIZE_NAME 100
#define MAGIC_NUMBER 9

int main(int argc, char* argv[]) {
    int size_of_file_name,res, i, fd2;
    char name[SIZE_NAME], file[SIZE_NAME], file_array[SIZE_MAX];
    struct utimbuf a_m_time;
    mode_t rights;
    time_t a_time, m_time;
    off_t size_of_file;
    ssize_t result;

    res = mkdir(argv[1], S_IRWXU);     /*examines if the catalog already exists and if not it creates it.*/ 
    if(errno == EEXIST) {
        fprintf(stderr, "Directory exists");
        exit(1);
    }
    examine(res, __LINE__);   

    while(1){           
            for(i = 0; i < SIZE_NAME; i++) {
            name[i] = '\0';
            file[i] = '\0';
            }
        
        result = my_read(STDIN_FILENO, &size_of_file_name, sizeof(int), __LINE__);
        if( result == 0) {
            break;
        }
        
        result = my_read(STDIN_FILENO, name , size_of_file_name, __LINE__);
        strcpy(file, argv[1]);        /*puts the name of the catalog with the name of the file.*/ 
        strcat(file, "/");          
        strcat(file, name);
        result = my_read(STDIN_FILENO, &a_time, sizeof(time_t), __LINE__);
        result = my_read(STDIN_FILENO, &m_time, sizeof(time_t), __LINE__);
        
        result = my_read(STDIN_FILENO, &rights, sizeof(mode_t), __LINE__);
        
        result = my_read(STDIN_FILENO, &size_of_file, sizeof(off_t), __LINE__);
//         fprintf(stderr, "p2unarchive before open atmine %lu modtime %lu\n", a_time, m_time);

        
        fd2 = open(file,  O_RDWR | O_CREAT, S_IRWXU);
        examine(fd2, __LINE__);
        while(size_of_file > SIZE_MAX) {            /*Transfering the contents of the file in blocks of maximum 512 bytes*/
            result = my_read(STDIN_FILENO, file_array, SIZE_MAX, __LINE__);
            my_write(fd2, file_array, SIZE_MAX, __LINE__);
            
            size_of_file -= 512;
        }

        if(size_of_file > 0) {
            my_read(STDIN_FILENO, file_array, size_of_file,__LINE__ );
            my_write(fd2, file_array, size_of_file, __LINE__);
        }
        

        res = close(fd2);
        examine(res, __LINE__);
        
        res = chmod(file, rights);      /*changes the rights of the file*/
        examine(res, __LINE__);
        
        a_m_time.actime = a_time;
        a_m_time.modtime = m_time;
//         fprintf(stderr, "p2unarchive after open atmine %lu modtime %lu\n", a_m_time.actime, a_m_time.modtime);
        res = utime(file, &a_m_time);     /*changes the access and the modification time of the file*/
        
        examine(res, __LINE__);
        
        
    }
    
	return(0);
}
