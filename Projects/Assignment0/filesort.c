#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>

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
stringNode* initalization(char* buffer, char* delimiters, int bufferSize, int delimiterSize, int filedescriptor, int bytesToRead);
void printLL(stringNode* head);
numberNode* numberCreator();
void printNLL(numberNode* head);
void freeSNode(stringNode* node);
int strcomp(void* string1, void* string2);
int insertionSort(void* head, int (*comparator)(void*, void*));
void* partition(void* startNode, void* endNode, int (*comparator)(void*, void*));
void quickSortRecursive(void* startNode, void* endNode, int (*comparator)(void*, void*));
int quickSort( void* head, int (*comparator)(void*, void*));
int intCompare(void* arg1, void* arg2);
void freeNNode(numberNode* node);
void freeNLL(numberNode* head);
void freeSLL(stringNode* head);

int numFile = 0;

int main(int argc, char** argv){

	if(argc != 3){
		printf("Fatal: Error: Requires exactly 3 arguments. (Executable, sorting, fileToSort)\n");
		return 0;
	}
	
	if(strlen(argv[1]) != 2 || argv[1][0] != '-' || (argv[1][1] != 'q' && argv[1][1] != 'i')){
		printf("Fatal Error: Please Input -i or -q only.\n");
		return 0;
	}
	
	int filedescriptor = open(argv[2], O_RDONLY);
	if(filedescriptor == -1){
		printf("Fatal Error: File does note exist. Please try again!\n");
		return 0;
	}

	
	char delimiters[1] = {','};
	char buffer[100] = {'\0'};
	int bytesToRead = sizeof(buffer);
	stringNode* head = initalization(buffer, delimiters, sizeof(buffer), sizeof(delimiters), filedescriptor, bytesToRead);
	
	if(head == NULL){
		printf("Warning: Empty File\n");
		return 0;
	}
	
	/*printf("The LL(Tokens) for the file is:\n");
	printLL(head);
	*/
	stringNode* _checkAllNull = head;
	while(_checkAllNull != NULL && strlen(_checkAllNull->value) == 0 ){
		_checkAllNull = _checkAllNull->next;
	}
	if(_checkAllNull == NULL){
		printf("Warning: file contains only empty tokens, treating it as strings.\n");
	}
	numberNode* nhead = NULL;
	if(numFile){
		stringNode* curNode = head;
		numberNode* curNNode = numberCreator();
		if(curNNode == NULL){
			freeSLL(head);
			return 0;
		}
		head = NULL;
		nhead = curNNode;
		
		while(curNode != NULL){
			long int valueHolder = atoll(curNode->value);
			if(valueHolder <= 2147483647 && valueHolder >= -2147483648){
				curNNode->value = atoi(curNode->value);
			}else{
				printf("FATAL ERROR: Integer Overflow\n");
				freeNLL(nhead);
				freeSLL(curNode);
				return 0;
			}
			
			if(curNode->next != NULL){
				numberNode* temp = numberCreator();
				if(temp == NULL){
					freeNLL(nhead);
					freeSLL(curNode);
					close(filedescriptor);
					return 0;
				}
				curNNode->next = temp;
				temp->prev = curNNode;
				curNNode = temp;
			}
			
			stringNode* toFree = curNode;
			curNode = curNode->next;
			freeSNode(toFree);
		}
	}
	
	/*printf("The LL for the numberNode is:\n");
	printNLL(nhead);
	
	
	printf("The LL for the stringNode is:\n");
	printLL(head);
	

	printf("The LL for the sorted is:\n");
	*/
	
	if(argv[1][1] == 'i') {
        if (numFile) {
            insertionSort(nhead, intCompare);
            printNLL(nhead);
            freeNLL(nhead);
            
        } else {
            insertionSort(head, strcomp);
            printLL(head);
            freeSLL(head);
        }
    } else {
        if (numFile) {
            quickSort(nhead, intCompare);
            printNLL(nhead);
            freeNLL(nhead);
            
        } else {
            quickSort(head, strcomp);
            printLL(head);
            freeSLL(head);
        }
	}
	
	if(close(filedescriptor) < 0){
		printf("Error: File Descriptor would not close\n");
	}
	
	return 0;
}
void printLL(stringNode* head){
	while(head != NULL){
		printf("%s\n", head->value);
		head = head->next; 
	}
}

void printNLL(numberNode* head){
	while(head != NULL){
		printf("%d\n", head->value);
		head = head->next;	
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
	free(node);
}

void freeSLL(stringNode* head){
	while(head != NULL){
		stringNode* temp = head;
		head = head->next;
		freeSNode(temp);
	}
}


void freeNNode(numberNode* node){
	if(node->prev != NULL){
		node->prev->next = node->next;
	}
	if(node->next != NULL){
		node->next->prev = node->prev;
	}
	free(node);
}

void freeNLL(numberNode* head){
	while(head != NULL){
		numberNode* temp = head;
		head = head->next;
		freeNNode(temp);
	}
}


int intCompare(void* arg1, void* arg2){
	numberNode* x1ptr = (numberNode*) arg1;
	numberNode* x2ptr = (numberNode*) arg2;
	int x1 = x1ptr->value;
	int x2 = x2ptr->value;
	
	if(x1 == x2){
		return 0;
	}else if(x1 < x2){
		return -1;
	}else{
		return 1;
	}
}

int insertionSort(void* head, int (*comparator)(void*, void*)){

	stringNode* ptr = head;
	stringNode* temp = NULL;
	while(ptr != NULL){
		temp = ptr;
		while(ptr->prev != NULL && comparator(ptr, ptr->prev) < 0 ){
			char* holder = ptr->value;
			ptr->value = ptr->prev->value;
			ptr->prev->value = holder;
			ptr = ptr->prev;
		}
		ptr = temp;
		ptr = ptr->next;
	}
	return 1;
}

void* partition(void* startNode, void* endNode, int (*comparator)(void*, void*)){
	stringNode* pivot = (stringNode*)startNode;
	
	stringNode* left = pivot->next;
   stringNode* end = endNode;
   stringNode* storeIndex = left;
	stringNode* beforeNULL = end;


    stringNode* i;
    for(i = left; i != end->next; i = i->next){
       if(comparator(i, pivot) <= 0){
       	  beforeNULL = storeIndex;
           char* holder = i->value;
           i->value = storeIndex->value;
           storeIndex->value = holder;
           storeIndex = storeIndex->next;
       }
    }
    char* holder = pivot->value;
    if(storeIndex == NULL){
    	pivot->value = beforeNULL->value;
    	beforeNULL->value = holder;
    	return beforeNULL;
    }else{
    	pivot->value = storeIndex->prev->value;
    	storeIndex->prev->value = holder;
    	return storeIndex->prev;
	 }
    return NULL; //ERROR IF IT REACHES HERE
}

void quickSortRecursive(void* startNode, void* endNode, int (*comparator)(void*, void*)){
    if(startNode == endNode || startNode == NULL || endNode == NULL){
        return;
    }

    stringNode* prevPivot = partition(startNode, endNode, comparator);
	if(prevPivot != startNode){
		quickSortRecursive(startNode, prevPivot->prev, comparator);
	}
    if(prevPivot != endNode){
		quickSortRecursive(prevPivot->next, endNode, comparator);
	}
   
}

int quickSort( void* head, int (*comparator)(void*, void*)){
    stringNode* tail = head;
    while(tail->next != NULL){
        tail = tail->next;
    }
    quickSortRecursive(head, tail, comparator);
	return 1;
}

int strcomp(void* string1, void* string2){
	stringNode* s1ptr = (stringNode*)string1;
	stringNode* s2ptr = (stringNode*)string2;
	
	char* s1 = s1ptr->value;
	char* s2 = s2ptr->value;
	while(*s1 != '\0' && *s2 != '\0'){
		if(*s1 == *s2){
			s1 = s1 + sizeof(char);
			s2 = s2 + sizeof(char);
		}else{
			return *s1 - *s2;
		}
	}
	return *s1 - *s2;
}

void readingFile(int fd, char* buffer, int bytesToRead){
    int position = 0;
    int bytesRead = 0;
    int status = 0;
    memset(buffer, '\0', bytesToRead);
    do{
        status = read(fd, (buffer + bytesRead), bytesToRead-bytesRead);
        if(status == 0){
            //printf("Finished reading the File or Buffer is filled\n");
            break;
        }else if(status == -1){
            printf("Error: error detected when file reading\n");
            return;
        }
        bytesRead += status;
    }while(bytesRead < bytesToRead);
}

stringNode* tokenCreator(int size){
	 int temp = 0;
    stringNode* newNode = (stringNode*) malloc(sizeof(stringNode) * 1);
    while(newNode == NULL && temp != 3){
    	printf("Error: Malloc Failed, retrying\n");
    	newNode = (stringNode*) malloc(sizeof(stringNode) * 1);
    	temp++;
    }
    if(temp == 3){
    printf("Fatal Error, ending program\n");
    	return NULL;
    }
    newNode->value = (char*) malloc(sizeof(char) * size);
    temp = 0;
    while(newNode->value == NULL && temp != 3){
    	printf("Error: Malloc Failed, retrying\n");
    	newNode->value = (char*) malloc(sizeof(char) * size);
    	temp++;
    }
    if(temp == 3){
    	printf("Fatal Error, ending program\n");
    	return NULL;
    }
    memset(newNode->value, '\0', sizeof(char) * size);
    newNode->next = NULL;
    newNode->prev = NULL;
    return newNode;
} 


numberNode* numberCreator(){
	numberNode* newNode = (numberNode*) malloc(sizeof(numberNode) * 1);
	int temp = 0;
	while(newNode == NULL && temp != 3){
		printf("Error: Malloc Failed, retrying\n");
		newNode = (numberNode*) malloc(sizeof(numberNode) * 1);
		temp++;
	}
	if(temp == 3){
    	printf("Fatal Error, ending program\n");
    	return NULL;
    }
	newNode->value = 0;
	newNode->next = NULL;
	newNode->prev = NULL;
	return newNode;
}

stringNode* initalization(char* buffer, char* delimiters, int bufferSize, int delimiterSize, int filedescriptor, int bytesToRead){
	  int defaultSize = 20;
     int position = 0;
     int bufferPos;
     int negative = 0;
     int fileType = 0;
     stringNode* head = tokenCreator(defaultSize+1);
     if(head == NULL){
     	 close(filedescriptor);
     	 exit(0);
     }
     stringNode* curNode = head;
     
     readingFile(filedescriptor, buffer, bytesToRead);
     while(buffer[0] != '\0'){
		  for(bufferPos = 0; bufferPos < (bufferSize/sizeof(buffer[0])); ++bufferPos){
		    int temp = 0;
		    int isDelimiter = 0;
		    for(temp = 0; temp < (delimiterSize/sizeof(delimiters[0])); ++temp){
		       if(buffer[bufferPos] == delimiters[temp]){
		           isDelimiter = 1;
		           break;
		       }
		    }

		    if(isDelimiter){ // && curNode->value[0] != '\0'
		     	if(negative && fileType){
                    int temp1 = 0;
                    char* negativeString =  (char*) malloc(sizeof(char) * (strlen(curNode->value) + 2)); // +2 one for terminal character and one for '-'
                    while(negativeString == NULL && temp1 != 3){
                        printf("Error: Malloc Failed, retrying\n");
                        negativeString = (char*) malloc(sizeof(char) * (strlen(curNode->value) + 2));
                        temp1++;
                    }
		     	   if(temp1 == 3){
		     			printf("Fatal Error: Malloc failed\n");
		     			freeSLL(head);
		     			close(filedescriptor);
		     			exit(0);
		     		}
				  	memset(negativeString, '\0', (strlen(curNode->value) + 2) * sizeof(char));
				 	negativeString[0] = '-';
				  	memcpy(negativeString+1, curNode->value, strlen(curNode->value));
				    	
				  	free(curNode->value);
				  	curNode->value = negativeString;
		     	}
		     		
		     	defaultSize = 20;
		     	position = 0;
		     	negative = 0;
		     		
		      stringNode* newNode = tokenCreator(defaultSize+1);
		      if(newNode == NULL){
		      	freeSLL(head);
		      	close(filedescriptor);
		      	exit(0);
		      }
		      newNode->prev = curNode;
		      curNode->next = newNode;
		         
		      curNode = newNode;
		         
		    }else if(isalnum(buffer[bufferPos]) == 0){ // isDelimiter &&
		     	if(buffer[bufferPos] == '-' && position == 0){
		     		negative = 1;
		     	}
		    }else{
		     	if(position >= defaultSize){
				  	defaultSize = defaultSize * 2;
                    int temp1 = 0;
                    char* expanded =  (char*) malloc(sizeof(char) * (defaultSize + 1));
                    while(expanded == NULL && temp1 != 3){
                        printf("Error: Malloc Failed, retrying\n");
                        expanded = (char*) malloc(sizeof(char) * (defaultSize + 1));
                        temp1++;
                    }
				  	if(temp1 == 3){
				  		printf("Fatal Error: Malloc failed\n");
				  		close(filedescriptor);
				  		freeSLL(head);
				  		exit(0);
				  	}
				  	memset(expanded, '\0', (defaultSize+1) * sizeof(char));
				  	memcpy(expanded, curNode->value, strlen(curNode->value));
				  	  	
				  	free(curNode->value);
				  	curNode->value = expanded;
		  	  	}
		      curNode->value[position] = buffer[bufferPos];
		      position++; 
		      if(isdigit(curNode->value[0]) > 0){
		  	  		fileType = 1;
		  	  		numFile = 1;	
		  	  	}
		    }
		     //END OF BUFFER LOOP
		 }
     	 readingFile(filedescriptor, buffer, bytesToRead);
	 }
	 if(negative && fileType){

         int temp1 = 0;
         char* negativeString =  (char*) malloc(sizeof(char) * (strlen(curNode->value) + 2)); // +2 one for terminal character and one for '-'
         while(negativeString == NULL && temp1 != 3){
             printf("Error: Malloc Failed, retrying\n");
             negativeString = (char*) malloc(sizeof(char) * (strlen(curNode->value) + 2));
             temp1++;
         }
	     if(temp1 == 3){
	         printf("Fatal Error: Malloc failed\n");
	         freeSLL(head);
	         close(filedescriptor);
	         exit(0);
	     }
	     memset(negativeString, '\0', (strlen(curNode->value) + 2) * sizeof(char));
	     negativeString[0] = '-';
	     memcpy(negativeString+1, curNode->value, strlen(curNode->value));
				    	
	     free(curNode->value);
	     curNode->value = negativeString;
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
} 
