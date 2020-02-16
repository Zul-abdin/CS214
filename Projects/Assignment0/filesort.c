#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct _stringNode_{
    char* name;
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
stringNode* tokenCreator(int size);
stringNode* initalization(char* buffer, char* delimiters, int buffersize, int delimitersize, int filedescriptor, int bytesToRead);
void printLL(stringNode* head);
void freeSNode(stringNode* node);
void printNLL(numberNode* head);
numberNode* numberCreator();



int sequentialSort(void* toSort, int (*comparator)(void*, void*)){
    int arrayLength = sizeof(toSort)/sizeof(toSort[0]); //This will not work since toSort will be a pointer to the start of the array so the size will be 8 instead of the size of the array (FIX LATER)
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
    newNode->name = (char*) malloc(sizeof(char) * size);
    memset(newNode->name, '\0', sizeof(char) * size);
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

            if(isDelimiter && curNode->name[0] != '\0'){
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

                    memcpy(expanded, curNode->name, defaultSize/2);

                    free(curNode->name);
                    curNode->name = expanded;
                }
                if(isspace(buffer[bufferPos]) == 0){
                    curNode->name[position] = buffer[bufferPos];
                    position++; 
                }
            }
        }
        memset(buffer, '\0', bytesToRead * sizeof(char));
        readingFile(filedescriptor, buffer, bytesToRead);
    }

    if(strlen(curNode->name) == 0){
        if(curNode->prev == NULL){
            head = NULL;
        }else{
            curNode->prev->next = NULL;
        }
        free(curNode);
    }

    return head;
} /* To add: make the name of the stringnodes be infinite long. 
Concept: Double the size of the char[] and memset the characters to the newly doubled array size (Inefficient?)

To add: Smart parsing (if it is a number, be able to store it into the numberLL etc.) Currently only works for strings
To add: removal of whitespace 
To add: ensure the name[] can be infinitely long (DONE)

 */

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
    printf("The LL for the stringNode is:\n");
	printLL(head);
	
	
	numberNode* nhead = NULL;

	if(isdigit(head->name[0]) > 0){
		stringNode* curNode = head;
        numberNode* curNNode = numberCreator();

		head = NULL;
		nhead = curNNode;

		while(curNode->next != NULL){
			curNNode->value = atoi(curNode->name);

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
    




    if(close(filedescriptor) < 0){
        printf("File Descriptor would not close\n");
    }
    return 0;
}

void printLL(stringNode* head){
    while(head != NULL){
        printf("%s\n", head->name);
        head = head->next;
    }
}

void printNLL(numberNode* node){
    while(node != NULL){
        printf("Value: %d\n", node->value);
        node = node->next;
    }
}

void freeSNode(stringNode* node){
    free(node->name);
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