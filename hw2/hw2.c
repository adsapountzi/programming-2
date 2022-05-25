/*An application that intergrates the contents of a directory into an encrypted archive and vice versa it decrypts it*/
/*Authors: A. Sapountzi aem: 02624
 * Date: 1/4/2018*/
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<sys/wait.h>
#include "myfunctions.h"
#define MAGIC_NUMBER 9

int main(int argc, char* argv[]) {
    int fd1, pfd1[2], pfd2[2], pid2, pid3, pid4, result, res;
    char magic_word[] = "P2CRYPTAR", str[MAGIC_NUMBER];
    
    
    res = strcmp(argv[1], "-E");      /*if the user selects -E to encrypt the catalog*/
    if(res == 0) {
    
        fd1 = open(argv[4], O_RDWR | O_CREAT | O_EXCL, S_IRWXU);        /*examines if the archive already exists and if not it creates it and it writes the magic word*/
        if(errno == EEXIST) {
            fprintf(stderr,"File already exists! line %d\n", __LINE__);
            exit(1);
        }
        if(fd1 == -1) {
            fprintf(stderr, "Error in open  line: %d\n",__LINE__);
            exit(1);
        }

        my_write(fd1, magic_word, MAGIC_NUMBER, __LINE__);

        result = pipe(pfd1);                    
        examine(result, __LINE__);
        pid2 = fork();              
        if(pid2 == 0){ 
            examine(close(fd1), __LINE__);
            result = dup2(pfd1[1], STDOUT_FILENO);
            examine(result, __LINE__);
            examine(close(pfd1[0]), __LINE__);
            examine(close(pfd1[1]), __LINE__);
            result = execlp("./dirlist","dirlist", argv[2], (char*)NULL);
            examine(result, __LINE__);
        }
        if(pid2 == -1) {
            fprintf(stderr,"Error, failed! line %d\n", __LINE__); 
            exit(1);
        }
                
        result = pipe(pfd2);
        examine(result, __LINE__);
            
        pid3 = fork();
        if(pid3 == -1){ 
            fprintf(stderr,"fork failed! line %d\n", __LINE__); 
            exit(1);
        }
        if(pid3 == 0){    
            examine(close(fd1), __LINE__);
            result = dup2(pfd1[0], STDIN_FILENO);
            examine(result, __LINE__);
            examine(close(pfd1[0]), __LINE__);
            examine(close(pfd1[1]), __LINE__);
            result = dup2(pfd2[1], STDOUT_FILENO);
            examine(result, __LINE__);
            examine( close(pfd2[0]), __LINE__);
            examine(close(pfd2[1]), __LINE__);
            result = execlp("./p2archive","p2archive", (char*)NULL);
            examine(result, __LINE__);
        }


        pid4 = fork();
        if(pid4 == -1){ 
            fprintf(stderr,"fork failed! line %d\n", __LINE__); 
            exit(1);
        }
        if(pid4 == 0){ 
            result = dup2(pfd2[0], STDIN_FILENO);
            examine(result, __LINE__);
            examine(close(pfd2[0]), __LINE__);
            examine(close(pfd2[1]), __LINE__);
            result = dup2(fd1, STDOUT_FILENO);
            examine(result, __LINE__);
            examine(close(pfd1[0]), __LINE__);
            examine(close(pfd1[1]), __LINE__);
            result = execlp("./p2crypt","p2crypt", argv[3], (char*)NULL);
            examine(result, __LINE__);
        }
        
        examine(close(fd1), __LINE__);
        examine(close(pfd1[0]), __LINE__);
        examine(close(pfd1[1]), __LINE__);
        examine(close(pfd2[0]), __LINE__);
        examine(close(pfd2[1]), __LINE__);
        waitpid(-1, NULL, 0); /* the parent waits for the kids to terminate.*/
        waitpid(-1, NULL, 0);
        waitpid(-1, NULL, 0);
    }


    res = strcmp(argv[1], "-D"); 
    if(res == 0) {
        fd1 = open(argv[4], O_RDWR, 0); /*examines if  encrypted archive exists.*/
        if(errno == ENOENT) {
            fprintf(stderr,"Error the file path does not exist\n");
            exit(1);
        }
        examine(fd1, __LINE__);
        my_read(fd1, str, MAGIC_NUMBER, __LINE__);
        
        result = strcmp(str, magic_word); /*examine if the magic number exists.*/
        if(result != 0) {
            fprintf(stderr,"Wrong magic number\n");
            exit(1);
        }
        result = pipe(pfd1);
        examine(result, __LINE__);
        
        pid2 = fork();
        if(pid2 == 0){ 
            result = dup2(fd1, STDIN_FILENO);
            examine(result, __LINE__);
            examine(close(fd1), __LINE__);
            result = dup2(pfd1[1], STDOUT_FILENO);
            examine(result, __LINE__);
            examine(close(pfd1[0]), __LINE__);
            examine(close(pfd1[1]), __LINE__);
            result = execlp("./p2crypt", "p2crypt", argv[3], (char*)NULL);
            examine(result, __LINE__);
        }
        if(pid2 == -1){
            fprintf(stderr,"Error, failed! line %d\n", __LINE__); 
            exit(1);
        }
        
        pid3 = fork();
        if(pid3 == 0){ 
            examine(close(fd1), __LINE__);
            result = dup2(pfd1[0], STDIN_FILENO);
            examine(result, __LINE__);
            examine(close(pfd1[0]), __LINE__);
            examine(close(pfd1[1]), __LINE__);
            result = execlp("./p2unarchive", "p2unarchive", argv[2], (char*)NULL);
            examine(result, __LINE__);
        }
        if(pid3 == -1){
            fprintf(stderr,"Error, failed! line %d\n", __LINE__); 
            exit(1);
        }
        
        examine(close(pfd1[0]), __LINE__);
        examine(close(pfd1[1]), __LINE__);
        examine(close(fd1), __LINE__);
        waitpid(-1, NULL,0); /* the parent waits for the kids to terminate.*/
        waitpid(-1, NULL, 0);
       
    }        
    return(0);
}
            
