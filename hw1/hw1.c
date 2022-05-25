/*A program that manages a base with digital objects. Specifically it imports and searches other files into the base, exports and deletes files. 
 * 
 * Authors: A. Sapountzi aem: 02624
 * Date: 11/3/2018
 */
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#define SIZE_MAX 512
#define SIZE_NAME 250

void examine(int result/*, int line */) {             /*A function that examines if the system call failed.*/
    if(result == -1) {
     /*   fprintf(stderr, "line: %d\n", line); */
        perror("Error ");
        exit(1);
    }
}

void import(int fd1) {                        /*a function that imports files into the base, if they do not exist already in the base*/
    int fd2, num_file_name, result, num_name, re, size_file, result2, i;
    char file_name[SIZE_NAME], short_name[SIZE_NAME], format[20], file_array[SIZE_MAX], str[] = "/", *ptr1, name[SIZE_NAME], array[SIZE_MAX], name_of_imported_file[SIZE_NAME];
    off_t size;
    
    for(i = 0; i < SIZE_NAME; i++) {
	name[i] = '\0';
    }
    
    sprintf(format, "%%%ds", SIZE_NAME - 1); 
    scanf(format, file_name);
    strcpy(name_of_imported_file, file_name);

    ptr1 = strstr(file_name, str);  /*Keeping the short name without the whole file path.*/
    if(ptr1 == NULL) {
        strcpy(short_name, name_of_imported_file);
    }
    else{
        do{ 
            strcpy(short_name, ptr1 + 1 );
            ptr1 = strstr(name_of_imported_file, str);
            if(ptr1 != NULL){
                *ptr1 = 'a';
            }
        }while(ptr1 != NULL);
    }
    result = lseek(fd1, (off_t)8, SEEK_SET);    
    examine(result);
    
    re =read(fd1, array, 1);  /*Examine if any file already exists in the base file.*/
    examine(re);
         
    if(re != 0) {
        result = lseek(fd1, (off_t)-1, SEEK_CUR);
        examine(result);
    
        do{
            result = read(fd1, &num_name, sizeof(int));          /*Examine if the file that we want to import already exists in the base.*/ 
            examine(result);
            
            result = read(fd1, name, num_name);
            examine(result);
	    
                
            result2 = strcmp(name, short_name);   
            
            if(result2 == 0) {
                printf("file already exists\n");
		return;
            }
             
	    for(i = 0; i < SIZE_NAME; i++) {
		name[i] = '\0';
	    }
	    
            result = read(fd1, &size_file, sizeof(off_t));
            examine(result);
            
            result = lseek(fd1, size_file, SEEK_CUR);
            examine(result);
            
            re = read(fd1, array, 1);          /*Examine if the file descriptor reached the end of the file base.*/
            examine(re);
            
            if(re == 0) {                   
                break;
            }
            result = lseek(fd1, (off_t)-1, SEEK_CUR);
            examine(result);
        }while(1);
    }	
                             /*transfering the size of the name, name, size of the contents and contents of the file into the base*/                 
    fd2 = open(file_name, O_RDONLY);
    if(fd2 == -1){
        perror("The file descriptor failed to open");
        exit(1);
    }

    size = lseek(fd2,(off_t)0, SEEK_END);  
    examine(size);
    
    num_file_name = strlen(short_name); 
    
    result = write(fd1, &num_file_name, sizeof(int));
    examine(result);

    result = write( fd1, short_name, num_file_name);
    examine(result);

    result = write(fd1, &size, sizeof(off_t));
    examine(result);

    result = lseek(fd2, (off_t)0, SEEK_SET);
    examine(result);

    while(size > SIZE_MAX) {            /*Transfering the contents of the file in blocks of maximum 512 bytes*/
        result = read(fd2, file_array, SIZE_MAX);
        examine(result);
        
        result = write(fd1, file_array, SIZE_MAX);
        examine(result);
        
        size -= 512;

    }

    result = read(fd2, file_array, size);
    examine(result);

    result = write(fd1, file_array, size);
    examine(result);    

    result = close(fd2);
    examine(result);
}

void find(int fd1) {                     /*A function that searches for all the objects in the base that include the user's search word in their name and prints their full names.*/
    char format[20], *ptr, find_name[SIZE_NAME], name[SIZE_NAME], array[20];
    int re, res,result, num_name, i;
	off_t size_file;
    
    sprintf(format, "%%%ds",SIZE_NAME - 1);
    scanf(format, find_name);
    
    result = lseek(fd1, (off_t)0, SEEK_SET);     
    examine(result);
    
    result = lseek(fd1, (off_t)8, SEEK_CUR);     
    examine(result);
    
    res = strcmp(find_name, "*");  /*If the user selects '*' , the program presents all the names of the base*/
    if(res == 0) {
        do{
	    for(i = 0; i < SIZE_NAME; i++) {
		name[i] = '\0';
	    }
            result = read(fd1, &num_name, sizeof(int));
            if(result == 0) {
                break;
            }
            examine(result);
            
            result = read(fd1, name, num_name);
            examine(result);                      
            
            printf("%s\n", name);
			
            result = read(fd1, &size_file, sizeof(off_t));
            examine(result);
            
            result = lseek(fd1, size_file, SEEK_CUR);
            examine(result);
            
            re =read(fd1, array , 1);  /*Examine if the file descriptor reached the end of the file base.*/
            examine(re);
            
            if(re == 0) {
                break;
            }
            result = lseek(fd1, (off_t)-1, SEEK_CUR);
            examine(result);
            
        }while(1);		
    }
    else{
        do{
	    for(i = 0; i < SIZE_NAME; i++) {
		name[i] = '\0';
	    }
            result = read(fd1, &num_name, sizeof(int));
            if(result == 0) {
                break;
            }
            examine(result);
            
            result = read(fd1, name, num_name);
            examine(result);
            
            ptr = strstr(name, find_name); /*examine if the search is included in any name of the base*/
			
            if(ptr != NULL) {
                printf("%s\n", name);
            }
         
            result = read(fd1, &size_file, sizeof(off_t));
            examine(result);
        
            result = lseek(fd1, size_file, SEEK_CUR);
            examine(result/*, __LINE__ */);
             
            re =read(fd1, array, 1);
            if(re == 0) {
                break;
            }
            examine(re);
          
            result = lseek(fd1, (off_t)-1, SEEK_CUR);
            examine(result);
        }while(1);
    }	
}

void export(int fd1) {  /*A function that exports the contents of any file of the base to another file.*/
    int fd3, result, re, res, destination_size, num_name = 0, i;
    char export_name[SIZE_NAME], format[20], format_str[20], destination_name[SIZE_NAME], contents[SIZE_MAX], name[SIZE_NAME], array[20];
    off_t size_file;
    
    for(i = 0; i < SIZE_NAME; i++) {
	name[i] = '\0';
    }
    
    printf("Enter the name of the file for exportion\n");
    sprintf(format, "%%%ds",SIZE_NAME - 1); 
    scanf(format, export_name);
    
    result = lseek(fd1, (off_t)8, SEEK_SET);     
    examine(result);
    
    do{
	result = read(fd1, &num_name, sizeof(int)); /*Examine if the file that the user wants to export exists in the base*/
	examine(result);
	
	result = read(fd1, name, num_name);
	examine(result);
            
	res = strcmp(name, export_name);
	
	for(i = 0; i < SIZE_NAME; i++) {
	    name[i] = '\0';
	}
	
	result = read(fd1, &size_file, sizeof(off_t));
	examine(result);
	
	if(res == 0) {
	    break;
	}
	
	result = lseek(fd1, size_file, SEEK_CUR);
	examine(result);
            
	re =read(fd1, array, 1);
	examine(re);
            
	if(re == 0) {
	    printf("The file you want to export does not exist\n");
	    return;
	}
	result = lseek(fd1, (off_t)-1, SEEK_CUR);
	examine(result);
    }while(1);
    
    printf("Enter the name of the file to be saved\n");
    sprintf(format_str, "%%%ds",SIZE_NAME - 1); 
    scanf(format_str, destination_name);
    
    fd3 = open(destination_name, O_RDWR | O_CREAT, S_IRWXU);
    if(fd3 == -1) {
	perror("The file descriptor failed to open");
        exit(1);
    }
    
    destination_size = lseek(fd3,(off_t)0, SEEK_END);   
    if(destination_size != 0) {
	printf("Error file already exists!\n");
        return;
    }
    
    /*Copies the contents of the file that the user wants to export to the new file*/
    
    while( size_file > SIZE_MAX) {
    result = read(fd1, contents, SIZE_MAX);
    examine(result);
    
    result = write(fd3, contents, SIZE_MAX);
    examine(result);
    
    size_file -= 512;
    }

    result = read(fd1, contents, size_file);
    examine(result);

    result = write(fd3, contents, size_file);
    examine(result);    
    
    result = close(fd3);
    examine(result);
}

void delete(int fd1, int size_base,int fd2) {  /*A function that deletes the contents of any file of the base.*/
    char format[20], delete_name[SIZE_NAME], name[SIZE_NAME], array[20], contents[SIZE_MAX];
    int result, res, i, re, pos, final_pos, final_size, num_name;
    off_t size_file, j;
    
    printf("Enter the name of the file that you want to delete.\n");
    sprintf(format, "%%%ds",SIZE_NAME - 1); 
    scanf(format, delete_name);
    
    result = lseek(fd1, (off_t)8, SEEK_SET);     
    examine(result);
    
    do{	
	for(i = 0; i < SIZE_NAME; i++) {     /*Examine if the file that the user wants to delete exists in the base*/
	    name[i] = '\0';
	}
	
	result = read(fd1, &num_name, sizeof(int));
        examine(result);
	
	result = read(fd1, name, num_name);
        examine(result);
            
	res = strcmp(name, delete_name);
	
	result = read(fd1, &size_file, sizeof(off_t));
        examine(result);
	
	result = lseek(fd1, size_file, SEEK_CUR);
        examine(result);
	
	if(res == 0) {
	    break;
	}

	re =read(fd1, array, 1);
        examine(result); 
        
	if(re == 0) {
	    printf("The file you want to delete does not exist\n");
	    return;
	}
	result = lseek(fd1, (off_t)-1, SEEK_CUR);
        examine(result);
    }while(1);
    
    pos = lseek(fd1, (off_t)0, SEEK_CUR);
    examine(pos);
    j = (pos - size_file - num_name - sizeof(int) - sizeof(off_t)); /*The position that indicates the start of the file that the user wants to delete*/
    
    result = lseek(fd2, j, SEEK_CUR);
    examine(result);
    
    final_size = size_base - pos;   /*The size of the files after the file that the user wants to delete*/
    
    while(final_size  > SIZE_MAX) {    /*Tranfering the files which exist after the file that the user wants to delete to the start of the delete file */
    
    result = read(fd1, contents, SIZE_MAX);
    examine(result);
    
    result = write(fd2, contents, SIZE_MAX);
    examine(result);
    
    final_size -= 512;
    }
    
    if(final_size != 0) {
    result = read(fd1, contents, final_size);
    examine(result);

    result = write(fd2, contents, final_size);
    examine(result);
    }
    
    final_pos = lseek(fd2, (off_t)0, SEEK_CUR);  /*The position that the base ends after the tranport*/
    examine(result);
    
    result = ftruncate(fd1, final_pos);
    examine(result);
    
    result = close(fd2);
    examine(result);
}

int main(int argc, char* argv[]) {
    int fd1, size_base, result, fd2;
    char filename[] = "deadcode", option;
    
    fd1 = open(argv[1], O_RDWR | O_CREAT, S_IRWXU);
    
    if(fd1 == -1) {
        perror("Error ");
        exit(1);
    }
    
    size_base = lseek(fd1, (off_t)0, SEEK_END);   /*if the base is just created , the magic word is added*/
    if(size_base == 0) {
	result = write(fd1, filename, 8);
        examine(result);
    }
    
    do{
        printf("i(mport)\nf(ind)\ne(xport)\nd(elete)\nq(uit)\n");
        
        scanf(" %c", &option);
	    
        if(option == 'i'){ 
            import(fd1);
	}
	if(option == 'f'){ 
	    find(fd1);
        }
        if(option == 'e'){
	    export(fd1);
        }
        if(option == 'd'){
	    size_base = lseek(fd1, (off_t)0, SEEK_END);  
	    examine(size_base);
	
	    fd2 = open(argv[1], O_RDWR);  /*Open the second file descriptor in the base.*/

	    if(fd2 == -1) {
		perror("Error: ");
		exit(1);
	    }
	    delete(fd1, size_base, fd2);
	    
        }
        if(option == 'q'){
            result = close(fd1);
            examine(result);
        }
    }
    while(option != 'q');
    
    return(0);
}

    
