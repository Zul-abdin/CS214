#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

typedef struct _stringNode_{
    char* value;
    struct _stringNode_* next;
    struct _stringNode_* prev;
}stringNode;

typedef struct _numberNode_{
    int value;
    struct _numberNode_* next;
    struct _numberNode_* prev;
}numberNode;

void readingFile(int fd, char* buffer, int bytesToRead);

int main(int argc, char* argv[]) {

    if(argc != 3){
        printf("Requires exactly 3 arguments. (Executable, sorting, fileToSort)\n");
        return 0;
    }

    if(strlen(argv[1]) != 2 || argv[1][0] != '-' || (argv[1][1] != 'q' && argv[1][1] != 'i')){
        printf("Please input only -i or -q!\n");
        return 0;
    } //NOTE change strcmp once we create the function 

    int filedescriptor = open(argv[2], O_RDONLY);
    if(filedescriptor == -1){
        printf("Error, File does not exist. Please try again!\n");
        return 0;
    }

    char delimiters[1] = {','};
    char buffer[100] = {'\0'};
    int bytesToRead = sizeof(buffer);
    do{
        readingFile(filedescriptor, buffer, bytesToRead);
    }while(buffer[0] != '\0');
    
    return 0;
}

void readingFile(int fd, char* buffer, int bytesToRead){
    int position = 0;
    int bytesRead = 0;
    int status = 0;
    memset(buffer, '\0', bytesToRead);
    do{
        status = read(fd, &buffer[position], bytesToRead-bytesRead);
        printf("Before Checking: Status is: %d\n", status);
        if(status == 0){
            break;
        }else if(status == -1){
            printf("Error in the file reading\n");
            return;
        }
        printf("Status is: %d\n", status);
        bytesRead += status;
        position = bytesRead; //Position is used to see where to place it in buffer so no overwrites
    }while(bytesRead < bytesToRead);
    int i = 0;
    for(i = 0;  i < 100; i++){
        printf("%c", buffer[i]);
    }
}
