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
stringNode* tokenCreator(int size);
stringNode* initalization(char* buffer, char* delimiters, int buffersize, int delimitersize, int filedescriptor, int bytesToRead);
void printLL(stringNode* head);
numberNode* numberCreator();
void printNLL(numberNode* head);
void freeSNode(stringNode* node);
int strcomp(void* arg1, void* arg2);
int intCompare(void*, void*);
int insertionSort(void* head, int (*comparator)(void*, void*));

int numFile = 0;

int quickSort(void* toSort, int (*comparator)(void*, void*)){

    return 0;
}

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
    stringNode* head = initalization(buffer, delimiters, sizeof(buffer), sizeof(delimiters), filedescriptor, bytesToRead);
    printf("The LL(Tokens) for the file is:\n");
	printLL(head);
	
	
	numberNode* nhead = NULL;

	if(isdigit(head->value[0]) > 0){
        numFile = 1;
		stringNode* curNode = head;
        numberNode* curNNode = numberCreator();

		head = NULL;
		nhead = curNNode;

		while(curNode->next != NULL){
			curNNode->value = atoi(curNode->value);

			numberNode* temp = numberCreator();
			curNNode->next = temp;
			temp->prev = curNNode;
			curNNode = temp;

			stringNode* toFree = curNode;
			curNode = curNode->next;
			freeSNode(toFree);
		}
	}
	
	printf("The LL for the numberNode is:\n");
	printNLL(nhead);
	
	
	printf("The LL for the stringNode is:\n");
	printLL(head);
    
    int (*compfp)(void*, void*);
    printf("Sorted:\n");
    if(numFile){
        compfp = intCompare;
        insertionSort(nhead, compfp);
        printNLL(nhead);
    }else{
        compfp = strcomp;
        insertionSort(head, compfp);
        printLL(head);
    }

    if(close(filedescriptor) < 0){
        printf("File Descriptor would not close\n");
    }
    return 0;
}

void printLL(stringNode* head){
    while(head != NULL){
        printf("%s\n", head->value);
        head = head->next;
    }
}

int intCompare(void* arg1, void* arg2){
    return (arg1 - arg2);
}

int strcomp(void* arg1, void* arg2){
    char* s1 = (char*)arg1;
    char* s2 = (char*)arg2;
    while(*s1 != '\0' && *s2 != '\0'){ 
        if(*s1 == *s2){
            s1 = s1 + sizeof(char);
            s2 = s2 + sizeof(char); //Move up one position in the array
        }else{
            return *s1 - *s2;
        }
    }
    return *s1 - *s2;
}

void readingFile(int fd, char* buffer, int bytesToRead){
    int position = 0;
    int bytesRead = 0;
    do{
        int status = read(fd, &buffer[position], bytesToRead-bytesRead);
        if(status == 0){
            printf("File Reading finished or Buffer is full\n");
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
    newNode->value = (char*) malloc(sizeof(char) * size);
    memset(newNode->value, '\0', sizeof(char) * size);
    newNode->next = NULL;
    newNode->prev = NULL;
    return newNode;
} //Change to generic so it can generate numberNodes

stringNode* initalization(char* buffer, char* delimiters, int buffersize, int delimitersize, int filedescriptor, int bytesToRead){
    int defaultSize = 20;
    int position = 0;
    int bufferPos;
    stringNode* head = tokenCreator(defaultSize);
    stringNode* curNode = head;

    memset(buffer, '\0', bytesToRead * sizeof(char));
    readingFile(filedescriptor, buffer, bytesToRead);
    while(buffer[0] != '\0'){
        for(bufferPos = 0; bufferPos < (buffersize/sizeof(buffer[0])); ++bufferPos){
            int temp = 0;
            int isDelimiter = 0;
            for(temp = 0; temp < (delimitersize/sizeof(delimiters[0])); ++temp){
                if(buffer[bufferPos] == delimiters[temp]){
                    isDelimiter = 1;
                    break;
                }
            }

            if(isDelimiter && curNode->value[0] != '\0'){
                position = 0;
                defaultSize = 20;

                stringNode* newNode = tokenCreator(defaultSize);

                newNode->prev = curNode;
                curNode->next = newNode;
                
                curNode = newNode;

            }else if(isDelimiter){
                continue;
            }else{
                if(position >= defaultSize && isspace(buffer[bufferPos]) == 0){
                    defaultSize = defaultSize * 2;

                    char* expanded = (char*) malloc(sizeof(char) * defaultSize);
                    memset(expanded, '\0', defaultSize * sizeof(char));

                    memcpy(expanded, curNode->value, defaultSize/2);

                    free(curNode->value);
                    curNode->value = expanded;
                }
                if(isspace(buffer[bufferPos]) == 0){
                    curNode->value[position] = buffer[bufferPos];
                    position++; 
                }
            }
        }
        memset(buffer, '\0', bytesToRead * sizeof(char));
        readingFile(filedescriptor, buffer, bytesToRead);
    }

    if(strlen(curNode->value) == 0){
        if(curNode->prev == NULL){
            head = NULL;
        }else{
            curNode->prev->next = NULL;
        }
        freeSNode(curNode);
    }

    return head;
} /* To add: make the value of the stringnodes be infinite long. 
Concept: Double the size of the char[] and memset the characters to the newly doubled array size (Inefficient?)

To add: Smart parsing (if it is a number, be able to store it into the numberLL etc.) Currently only works for strings
To add: removal of whitespace 
To add: ensure the value[] can be infinitely long (DONE)

 */
void printNLL(numberNode* node){
    while(node != NULL){
        printf("Value: %d\n", node->value);
        node = node->next;
    }
}

void freeSNode(stringNode* node){
    free(node->value);
    if(node->prev != NULL){
        node->prev->next = node->next;
    }
    if(node->next != NULL){
        node->next->prev = node->prev;
    }
}

numberNode* numberCreator(){
    numberNode* newNode = (numberNode*) malloc(sizeof(numberNode) * 1);
    newNode->value = 0;
    newNode->next = NULL;
    newNode->prev = NULL;
    return newNode;
}

int insertionSort(void* head, int (*comparator)(void*, void*)){
    stringNode* ptr = head;
    stringNode* temp = NULL;
    stringNode* valueHolder = NULL;

    if(comparator != &strcomp){
        ptr = (numberNode*) ptr;
        temp = (numberNode*) temp;
        valueHolder = (numberNode*) malloc(sizeof(numberNode) * 1);
    }else{
        valueHolder = (stringNode*) malloc(sizeof(stringNode) * 1);
    }
    while(ptr != NULL){
        if(ptr->prev == NULL){ //First element is always sorted
            ptr = ptr->next;
            continue;
        }
        temp = ptr;
        while(ptr->prev != NULL && comparator(ptr->value, ptr->prev->value) < 0){
            valueHolder->value = ptr->value;
            ptr->value = ptr->prev->value;
            ptr->prev->value = valueHolder->value;
            ptr = ptr->prev;
        }
        ptr = temp;
        ptr = ptr->next;
    }
    return 1;
}