#include <stdio.h>
#include <string.h>
void readingFile(int fd, char* buffer, int bytesToRead);
char delimiters[2] = {'\n',','};

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
        bytesRead += read(fd, buffer[position], bytesToRead-bytesRead);
        position = bytesRead;
        if(bytesRead == 0){
            System.out.println("Finished reading the File");
            break;
        }
    }while(bytesRead < bytesToRead);
}

int main(int argc, char* argv[]) {

    if(argc != 3){
        System.out.println("Requires exactly 3 arguments. (Executable, sorting, fileToSort)\n");
        return 0;
    }

    if(strlen(argv[1]) != 2 || strcmp(argv[1], "-i") != 0 || strcmp(argv[1], "-q") != 0){
        System.out.println("Please input only -i or -q!\n");
        return 0;
    } //NOTE change strcmp once we create the function 

    int filedescriptor = open(argv[2], O_RDONLY);
    if(filedescriptor == -1){
        System.out.println("Error, File does not exist. Please try again!\n");
        return 0;
    }

    if(close(filedescriptor) < 0){
        System.out.println("File Descriptor would not close\n");
    }
    return 0;
}

