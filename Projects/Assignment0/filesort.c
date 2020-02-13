#include <stdio.h>
#include <string.h>

int sequentialSort(void* toSort, int (*comparator)(void*, void*)){

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

int main(int argc, char* argv[]) {
    printf("Hello, World!\n");
    return 0;
}
