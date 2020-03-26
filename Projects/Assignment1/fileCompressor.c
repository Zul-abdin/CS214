#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>

typedef struct _NODE_{
	char* data;
	struct _NODE_* next;
	struct _NODE_* prev;
	int frequency;
}node;

typedef struct _TREENODE_{
    char* data;
    int frequency;
    struct _TREENODE_* left;
    struct _TREENODE_* right;
}treeNode;

typedef struct _LLNODE_{
    struct _TREENODE_* data;
    struct _LLNODE_* next;
}LLNode;

int itemCount = 0;
int sizeHT = 1000;
node* hashT[1000] = {NULL};

LLNode* head = NULL;

void directoryTraverse(char* path, int recursive, int mode);
void bufferFill(int fd, char* buffer, int bytesToRead);
void tokenCreate(char* path, char* buffer, char* delimiters, int bufferSize, int delimiterSize);
char* pathCreator(char* path, char* name);
void freeNode(node* node);
void insertHT(char* word);
void printHT();
treeNode* createTreeNode(char* data, int frequency, treeNode* left, treeNode* right);
void initializeLL();
void LLSortedInsert(LLNode* newNode);
void processLL();
void processTree(treeNode* root, char* bitString);
int isLeaf(treeNode* tNode);

int main(int argc, char** argv){
	if(argc != 4 && argc != 5){
		printf("Fatal Error: too many or too few arguments\n");
		return 0;
	}
	if(strlen(argv[1]) == 2 && argv[1][0] == '-' ){
			if(argv[1][1] == 'R'){
				if(strlen(argv[2]) == 2 && argv[2][0] == '-'){
					if(argv[2][0] == '-'){
						if(argv[2][1] == 'b' && argc == 4){ //build recursion
							char* _pathlocation_ = malloc(sizeof(char) * (strlen(argv[3]) + 1));
							memcpy(_pathlocation_, argv[3], strlen(argv[3]));
							_pathlocation_[strlen(argv[3])] = '\0';
							directoryTraverse(_pathlocation_, 1, 0);
						}else if(argv[2][1] == 'c' && argc == 5){ //compress recursion
						
						}else if(argv[2][1] == 'd' && argc == 5){ //decompress recursion
							
						}else{
							printf("Fatal Error: Wrong Arguments\n");
							return 0;
						}
					}else{
						printf("Fatal Error: Wrong Arguments\n");
						return 0;
					}
				}else{
					printf("Fatal Error: Wrong Arguments\n");
					return 0;
				}
			}else if(argv[1][1] == 'b' && argc == 4){ //build mode no recursion
					char* _pathlocation_ = malloc(sizeof(char) * (strlen(argv[2]) + 1));
					memcpy(_pathlocation_, argv[2], strlen(argv[2]));
					_pathlocation_[strlen(argv[2])] = '\0';
					directoryTraverse(_pathlocation_, 0, 0);
			}else if(argv[1][1] == 'c' && argc == 4){ //compress no recursion
				//decompress algorithm
			}else if(argv[1][1] == 'd' && argc == 4){ //decompress no recursion
				//compress algorithm
			}else{
				printf("Fatal Error: Wrong Arguments\n");
				return 0;
			}
	}else{
		printf("Fatal Error: Wrong Arguments\n");
		return 0;
	}
	printHT();
	printf("%d\n", itemCount);

	initializeLL();
	processLL();
	processTree(head->data, "");

	return 0;
}
/*
	DirectoryTraverse modes:
	0 = build
	1 = compress
	2 = decompress
*/
void directoryTraverse(char* path, int recursive, int mode){ 
	DIR* dirPath = opendir(path);
	if(!dirPath){
		printf("Fatal Error: Directory path does not exist!\n");
		closedir(dirPath);
		free(path);
		exit(0);
	}
	struct dirent* curFile = readdir(dirPath);
	while(curFile != NULL){
		if(strcmp(curFile->d_name, ".") == 0 || strcmp(curFile->d_name, "..") == 0){
			curFile = readdir(dirPath);
			continue;
		}
		if(curFile->d_type == DT_REG){
			printf("File Found: %s\n", curFile->d_name);
			char* filepath = pathCreator(path, curFile->d_name);
			printf("File path: %s\n", filepath);
			if(mode == 0){
				char delimiters[2] = {' ', '\n'};
				char buffer[100] = {'\0'};
				tokenCreate(filepath, buffer, delimiters, sizeof(buffer), sizeof(delimiters));
			}
			
		}else if(curFile->d_type == DT_DIR && recursive){
			printf("Directory Found: %s\n", curFile->d_name);
			char* directorypath = pathCreator(path, curFile->d_name);
			printf("Directory Location: %s\n", directorypath);
			directoryTraverse(directorypath, recursive, mode);
		}else{
			
		}
		curFile = readdir(dirPath);
	}
	free(path);
	closedir(dirPath);
}

char* pathCreator(char* path, char* name){
	char* newpath = (char *) malloc(sizeof(char) * (strlen(path) + strlen(name) + 2));
	memcpy(newpath, path, strlen(path));
	newpath[strlen(path)] = '/';
	memcpy((newpath + strlen(path) + 1), name, (strlen(name)));
	newpath[strlen(newpath)] = '\0';
	return newpath;
}

void tokenCreate(char* path, char* buffer, char* delimiters, int bufferSize, int delimiterSize){
	int fd = open(path, O_RDONLY);
	if(fd == -1){
		printf("Error: File does not exist: should not be possible\n");
		return;
	}
	bufferFill(fd, buffer, bufferSize);
	
	int bufferPos;
	int defaultSize = 20;
	char* word = (char*) malloc(sizeof(char) * defaultSize);
	word = memset(word, '\0', sizeof(char) * defaultSize);
	int wordpos = 0;
	
	while(buffer[0] != '\0'){
		for(bufferPos = 0; bufferPos < (bufferSize/sizeof(buffer[0])); ++bufferPos){
			/*int delimiterPos;
			int isDelimiter = 0;
			for(delimiterPos = 0; delimiterPos < (delimiterSize/sizeof(delimiters[0])); ++delimiterPos){
				if(buffer[bufferPos] == delimiters[delimiterPos]){
					isDelimiter = 1;
					break;
				}
			}
			*/
			if(isspace(buffer[bufferPos])){
				char* whitespaceholder = (char*) malloc(sizeof(char) * (1 + 1)); //One character for the whitespace character and one for terminal character for strings
				memset(whitespaceholder, '\0', (sizeof(char) * (1 + 1)));
				memcpy(whitespaceholder, (buffer + bufferPos), sizeof(char));
				
				insertHT(whitespaceholder);
				insertHT(word);
				
				defaultSize = 20;
				wordpos = 0;
				word = (char *)malloc(sizeof(char) * (defaultSize + 1));
				memset(word, '\0', (sizeof(char) * (defaultSize + 1)));
				
			}else{
				if(wordpos >= defaultSize){
				  	defaultSize = defaultSize * 2;
				  	char* expanded =  (char*) malloc(sizeof(char) * (defaultSize + 1));
				  	memset(expanded, '\0', (defaultSize+1) * sizeof(char));
				  	memcpy(expanded, word, strlen(word));
				  	free(word);
				  	word = expanded;
		  	  	}else{
		  	  		word[wordpos] = buffer[bufferPos];
		  	  		wordpos++;
		  	  	}
			}
		}
		bufferFill(fd, buffer, bufferSize);	
	}
	if(word[0] != '\0'){ //Gets the last token if it does not end with a delimiter (treats EOF as delimiter)
		insertHT(word);
	}else{
		free(word);
	}
	if(close(fd) < 0){
		printf("Warning: File Descriptor would not close\n");
	}
}

int getKey(char* word, int size){
	int loop;
	int sum = 0;
	while(*word){
		sum += atoi(word);
		word++;
	}
	int index = sum % size;
	if(index < 0){
		printf("Shouldn't ever occur\n");
		index *= -1;
	}
	return index;
}

void insertHT(char* word){
	int index = getKey(word, sizeHT);
	node* newNode = (node *)malloc(sizeof(node) * 1);
	newNode->data = word;
	printf("%s\n", newNode->data);
	newNode->next = NULL;
	newNode->prev = NULL;
	newNode->frequency = 1;
	
	if(hashT[index] != NULL){
		node* curNode = hashT[index];
		node* prevNode = curNode;
		while(curNode != NULL){
			if(strcmp(curNode->data, newNode->data) == 0){
				curNode->frequency = curNode->frequency + 1;
				freeNode(newNode);
				return;
			}
			prevNode = curNode;
			curNode = curNode->next;
		}
		prevNode->next = newNode;
		newNode->prev = prevNode;
	}else{
		hashT[index] = newNode;
	}
	itemCount++;
}

void printHT(){
	int i = 0;
	for(i = 0; i < sizeHT; i++){
	    node* curr = hashT[i];
		if(curr != NULL){
			while(curr != NULL){
				printf("%s with frequency %d\n", curr->data, curr->frequency);
                curr = curr->next;
			}
		}
	}
}

void freeNode(node* curNode){
	free(curNode->data);
	free(curNode);
}

void bufferFill(int fd, char* buffer, int bytesToRead){
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
            printf("Warning: Error when reading the file reading\n");
            return;
        }
        bytesRead += status;
    }while(bytesRead < bytesToRead);
}

treeNode* createTreeNode(char* data, int frequency, treeNode* left, treeNode* right){
    treeNode* newNode = (treeNode*)malloc(sizeof(treeNode));
    newNode->data = data;
    newNode->frequency = frequency;
    newNode->left = left;
    newNode->right = right;
    return newNode;
}

void initializeLL(){
    int i = 0;
    for(i = 0; i < sizeHT; i++){
        if(hashT[i] != NULL){
            while(hashT[i] != NULL){
                LLNode* newNode = (LLNode*)malloc(sizeof(LLNode));
                newNode->data = createTreeNode(hashT[i]->data, hashT[i]->frequency, NULL, NULL);
                newNode->next = NULL;
                LLSortedInsert(newNode);
                hashT[i] = hashT[i]->next;
            }
        }
    }
}

void LLSortedInsert(LLNode* newNode){
    if(head == NULL || head->data->frequency >= newNode->data->frequency){
        newNode->next = head;
        head = newNode;
    } else {
        LLNode* curr = head;
        while(curr->next != NULL && curr->next->data->frequency < newNode->data->frequency){
            curr = curr->next;
        }
        newNode->next = curr->next;
        curr->next = newNode;
    }
}

void processLL(){
    while(head->next != NULL){
        LLNode* first = head;
        LLNode* second = head->next;
        treeNode* root = createTreeNode(NULL, first->data->frequency + second->data->frequency, first->data, second->data);
        LLNode* newNode = (LLNode*)malloc(sizeof(LLNode));
        newNode->data = root;
        newNode->next = NULL;
        LLSortedInsert(newNode);
        head = head->next->next;
        free(first);
        free(second);
    }
}

void processTree(treeNode* root, char* bitString){
    if(root == NULL){
        return;
    }
	 if(isLeaf(root)){
        printf("%s\t%s\n", root->data, bitString);
    }	

    char* leftBitString = (char*)malloc(strlen(bitString) + 2);
    memcpy(leftBitString, bitString, strlen(bitString) + 1);
    leftBitString[strlen(bitString)] = '0';
    leftBitString[strlen(bitString) + 1] = '\0';
    processTree(root->left, leftBitString);
    free(leftBitString);

    char* rightBitString = (char*)malloc(strlen(bitString) + 2);
    memcpy(rightBitString, bitString, strlen(bitString) + 1);
    rightBitString[strlen(bitString)] = '1';
    rightBitString[strlen(bitString) + 1] = '\0';
    processTree(root->right, rightBitString);
    free(rightBitString);
	
}

int isLeaf(treeNode* tNode){
    if(tNode->left == NULL && tNode->right == NULL){
        return 1;
    } else {
        return 0;
    }
}
