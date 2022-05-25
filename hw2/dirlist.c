#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<dirent.h>
#include "myfunctions.h"

int main(int argc, char* argv[]) {
    DIR *dip;
    struct dirent *dit;
    int result;
    
    dip = opendir(argv[1]);
    
    if(dip == NULL) {
        if( errno == ENOENT) { /*examines if the catalog already exists.*/
            printf("File does not exist.\n");
            return(0);
        }
        
        perror("opendir failed");
        exit(0);
    }
        
    while((dit = readdir(dip)) != NULL) {
        if(dit == NULL) {
            perror("readdir failed");
            exit(0);
        }
        
        if(dit->d_type == DT_REG){ /* prints regular files only.*/
            printf("%s/%s\n", argv[1], dit->d_name);
        }
    }
    
    result = closedir(dip);
    examine(result, __LINE__);
    return 0;
}
