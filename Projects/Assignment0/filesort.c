#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct _stringNode_{
    char* name;
    struct _stringNode_* next;
}stringNode;

typedef struct _numberNode_{
    int value;
    struct _numberNode_* next;
}numberNode;

void readingFile(int fd, char* buffer, int bytesToRead);
stringNode* tokenCreator(int size);
stringNode* tokenCreator(int size);
stringNode* initalization(char* buffer, char* delimiters, int buffersize, int delimitersize);

int sequentialSort(void* toSort, int (*comparator)(void*, void*)){
    int arrayLength = sizeof(toSort)/sizeof(toSort[0]);
    for(int sortedPosition = 1; sortedPosition < arrayLength; ++sortedPosition){
        int unsortedElementPos = sortedPosition + 1;
        while(comparator(toSort[unsortedElementPos], toSort[unsortedElementPos - 1]) < 0 && unsortedElementPos > 0){
            void* temp = toSort[unsortedElementPos - 1];
            toSort[unsortedElementPos - 1] = toSort[unsortedElementPos];
            toSort[unsortedElementPos] = temp;
            unsortedElementPos--;
        }
    }
    return 0;
}

int quickSort(void* toSort, int (*comparator)(void*, void*)){

    return 0;
}

int intCompare(void* arg1, void* arg2){
    int ar1 = *(int*)arg1;
    int ar2 = *(int*)arg2;
    printf("%d", ar1 - ar2);
    return ar1 - ar2;
}

int strCompare(void* arg1, void* arg2){
    char* ar1 = (char*)arg1;
    char* ar2 = (char*)arg2;
    printf("%d", strcmp(ar1, ar2));
//TODO Replace strcmp() with a self-made string comparing function
    return strcmp(ar1, ar2);
}

void readingFile(int fd, char* buffer, int bytesToRead){
    int position = 0;
    int bytesRead = 0;
    do{
        int status = read(fd, &buffer[position], bytesToRead-bytesRead);
        if(status == 0){
            printf("Finished reading the File\n");
            break;
        }else if(status == -1){
            printf("Error in the file reading\n");
        }
        bytesRead += status;
        position = bytesRead; //Position is used to see where to place it in buffer so no overwrites
    }while(bytesRead < bytesToRead);
}

stringNode* tokenCreator(int size){
    stringNode* newNode = (stringNode*) malloc(sizeof(stringNode) * 1);
    newNode->name = (char*) malloc(sizeof(char) * size);
    memset(newNode->name, '\0', sizeof(char) * size);
    newNode->next = NULL;
    return newNode;
} //Change to generic so it can generate numberNodes

stringNode* initalization(char* buffer, char* delimiters, int buffersize, int delimitersize){
     int position = 0;
     int bufferPos;
     stringNode* head = tokenCreator(20);
     stringNode* curNode = head;
     for(bufferPos = 0; bufferPos < (buffersize/sizeof(buffer[0])); ++bufferPos){
        int temp = 0;
        int isDelimiter = 0;
        for(temp = 0; temp < (delimitersize/sizeof(delimiters[0])); ++temp){
            if(buffer[bufferPos] == delimiters[temp]){
                isDelimiter = 1;
                break;
            }
        }
        if(isDelimiter && curNode->name[0] != '\0'){
            stringNode* newNode = tokenCreator(20);
            curNode->next = newNode;
            curNode = newNode;
            position = 0;
        }else{
            curNode->name[position] = buffer[bufferPos];
            position++; 
        }
     }
    return head;
} /* To add: make the name of the stringnodes be infinite long. 
Concept: Double the size of the char[] and memset the characters to the newly doubled array size (Inefficient?)

To Add: Concept for File Reading: READ TILL you find delimiter once find delimiter, store. 
Currently, file reading will parse only 100 words each.
 */

int main(int argc, char* argv[]) {

    if(argc != 3){
        printf("Requires exactly 3 arguments. (Executable, sorting, fileToSort)\n");
        return 0;
    }

    if(strlen(argv[1]) != 2 || strcmp(argv[1], "-i") != 0 || strcmp(argv[1], "-q") != 0){
        printf("Please input only -i or -q!\n");
        return 0;
    } //NOTE change strcmp once we create the function 

    int filedescriptor = open(argv[2], O_RDONLY);
    if(filedescriptor == -1){
        printf("Error, File does not exist. Please try again!\n");
        return 0;
    }

    
    if(close(filedescriptor) < 0){
        printf("File Descriptor would not close\n");
    }
    return 0;
}

