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
	int whitespace;
}node;

typedef struct _TREENODE_{
    char* data;
    int frequency;
    int whitespace;
    struct _TREENODE_* left;
    struct _TREENODE_* right;
}treeNode;

typedef struct _LLNODE_{
    struct _TREENODE_* data;
    struct _LLNODE_* next;
}LLNode;

int itemCount = 0;
int sizeHT = 10;
node** hashT = NULL;

LLNode* head = NULL;

void directoryTraverse(char* path, int recursive, int mode);
void bufferFill(int fd, char* buffer, int bytesToRead);
void fileReading(char* path, char* buffer, int bufferSize, int mode);
char* pathCreator(char* path, char* name);
void freeNode(node* node);
void insertHT(char* word, int whitespace, int frequency, int rehashing);
void printHT();
treeNode* createTreeNode(char* data, int frequency, treeNode* left, treeNode* right, int whitespace);
void initializeLL();
void LLSortedInsert(LLNode* newNode);
void processLL();
void processTree(treeNode* root, char* bitString, int fd);
int isLeaf(treeNode* tNode);
void freeHT(node** head);
void rehash();
void freeTreenode(treeNode* root);
void freeLLnode(LLNode* node);
void writeToFile(char* word, int bytesToWrite, int whitespace, int fd);
void fileWriting(char*);
char* sequenceReplace(char*);

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
							hashT = (node**) calloc(sizeHT, sizeof(node*));
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
					hashT = (node**) calloc(sizeHT, sizeof(node*));
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
	//processTree(head->data, "");
	fileWriting("HuffmanCodebook.txt");
	freeHT(hashT);
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
				//char delimiters[2] = {' ', '\n'};
				char buffer[100] = {'\0'};
				fileReading(filepath, buffer, sizeof(buffer), mode);
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
/*
	fileReading modes:
	0 -> build
	1 -> compress
	2 -> decompress
*/
void fileReading(char* path, char* buffer, int bufferSize, int mode){ //Old Template: void fileReading(char* path, char* buffer, char* delimiters, int bufferSize, int delimiterSize);
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
				if(mode == 0){
					char* whitespaceholder = (char*) malloc(sizeof(char) * (1 + 1)); //One character for the whitespace character and one for terminal character for strings
					memset(whitespaceholder, '\0', (sizeof(char) * (1 + 1)));
					memcpy(whitespaceholder, (buffer + bufferPos), sizeof(char));
				
					insertHT(whitespaceholder, 1, 1, 0);
					insertHT(word, 0, 1, 0);
					if(itemCount == sizeHT){
						rehash();
					}
				}
				
				
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
	if(mode == 0 && word[0] != '\0'){ //Gets the last token if it does not end with a delimiter (treats EOF as delimiter) 
		insertHT(word, 0, 1, 0);
	}else if(mode == 1 && word[0] != '\0'){

	}else if(mode == 2 && word[0] != '\0'){
	
	}else{
		free(word);
	}
	if(close(fd) < 0){
		printf("Warning: File Descriptor would not close\n");
	}
}

int getKey(char* word, int size){
	int loop = 0;
	int hash = 0;
	for(loop = 0; loop < strlen(word); ++loop){
		hash = ((hash << 5) - hash) + word[loop];
	}
	int index = hash % size;
	if(index < 0){
		printf("Shouldn't ever occur\n");
		index *= -1;
	}
	return index;
}

void rehash(){
	sizeHT = sizeHT * 2;
	node** newHT = (node**) calloc(sizeHT,sizeof(node*));
	node** oldHead = hashT;
	hashT = newHT;
	
	int i = 0;
	for(i = 0; i < (sizeHT/2); ++i){
		node* curNode = oldHead[i];
		while(curNode != NULL){
			insertHT(curNode->data, curNode->whitespace, curNode->frequency, 1);
			node* temp = curNode;
			curNode = curNode->next;
			free(temp);
		}
	}
	free(oldHead);
}

void insertHT(char* word, int whitespace, int frequency, int rehashing){
	int index = getKey(word, sizeHT);
	node* newNode = (node *)malloc(sizeof(node) * 1);
	newNode->data = word;
	printf("%s\n", newNode->data);
	newNode->next = NULL;
	newNode->prev = NULL;
	newNode->frequency = frequency;
	newNode->whitespace = whitespace;
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
	if(rehashing == 0){
		itemCount++;
	}
}

void printHT(){
	int i = 0;
	for(i = 0; i < sizeHT; i++){
	   node* curr = hashT[i];
		while(curr != NULL){
				printf("%s with frequency %d\n", curr->data, curr->frequency);
            curr = curr->next;
		}
	}
}

void freeHT(node** head){
	int i = 0;
	for(i = 0; i < sizeHT; i++){
		while(head[i] != NULL){
			node* temp = head[i];
			head[i] = head[i]->next;
			freeNode(temp);
		}
	}
	free(head);
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

treeNode* createTreeNode(char* data, int frequency, treeNode* left, treeNode* right, int whitespace){
    treeNode* newNode = (treeNode*)malloc(sizeof(treeNode));
    newNode->data = data;
    newNode->frequency = frequency;
    newNode->whitespace = whitespace;
    newNode->left = left;
    newNode->right = right;
    return newNode;
}

void initializeLL(){
    int i = 0;
    for(i = 0; i < sizeHT; i++){
    		node* hashTnode = hashT[i];
         while(hashTnode != NULL){
              LLNode* newNode = (LLNode*)malloc(sizeof(LLNode));
              newNode->data = createTreeNode(hashTnode->data, hashTnode->frequency, NULL, NULL, hashTnode->whitespace);
              newNode->next = NULL;
              LLSortedInsert(newNode);
              hashTnode = hashTnode->next;
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
        treeNode* root = createTreeNode(NULL, first->data->frequency + second->data->frequency, first->data, second->data, 0);
        LLNode* newNode = (LLNode*)malloc(sizeof(LLNode));
        newNode->data = root;
        newNode->next = NULL;
        head = head->next->next;
        free(first);
        free(second);
        LLSortedInsert(newNode);

    }
}
void freeLLnode(LLNode* node){
	freeTreenode(node->data);
	free(node);
}

void freeTreenode(treeNode* root){	
	/*if(root == NULL){ //some error checking
		return;
	}
	*/
	free(root->data);
	if(root->left != NULL){
		freeTreenode(root->left);
	}
	if(root->right != NULL){
		freeTreenode(root->right);
	}
	free(root);
}

char* sequenceReplace(char* word){
	char* temp = (char*) malloc(sizeof(char) * strlen(word) + 2);
	memset(temp, '\0', strlen(word) + 2 * sizeof(char));
	temp[0] = '\\';
	memcpy(temp+1, word, strlen(word));
	free(word);
	return temp;
}
void fileWriting(char* filename){
	int fd = open(filename, O_WRONLY | O_TRUNC | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
	writeToFile("ESCAPESEQ", strlen("ESCAPESEQ"), 0, fd);
	writeToFile("\n", strlen("\n"), 0, fd);
	processTree(head->data, "", fd);
	close(fd);
}
void writeToFile(char* word, int bytesToWrite, int whitespace, int fd){
	int bytesWritten = 0;
	int status = 0;
	if(whitespace){
		writeToFile("ESCAPESEQ", strlen("ESCAPESEQ"), 0, fd);
		word = sequenceReplace(word);
	}
	while(bytesWritten < bytesToWrite){
		status = write(fd, (word + bytesWritten), (bytesToWrite - bytesWritten));
		if(status == -1){
			printf("Error: write failed\n");
			return;
		}
		bytesWritten += status;
	}
}

void processTree(treeNode* root, char* bitString, int fd){
    if(root == NULL){
        return;
    }
	 if(isLeaf(root)){
        printf("%s\t%s\n", root->data, bitString);
        writeToFile(root->data, strlen(root->data), root->whitespace, fd);
        writeToFile("\t", strlen("\t"), 0, fd);
        writeToFile(bitString, strlen(bitString), 0, fd);
        writeToFile("\n", strlen("\n"), 0, fd);
    }	

    char* leftBitString = (char*)malloc(strlen(bitString) + 2);
    memcpy(leftBitString, bitString, strlen(bitString) + 1);
    leftBitString[strlen(bitString)] = '0';
    leftBitString[strlen(bitString) + 1] = '\0';
    processTree(root->left, leftBitString, fd);
    free(leftBitString);

    char* rightBitString = (char*)malloc(strlen(bitString) + 2);
    memcpy(rightBitString, bitString, strlen(bitString) + 1);
    rightBitString[strlen(bitString)] = '1';
    rightBitString[strlen(bitString) + 1] = '\0';
    processTree(root->right, rightBitString, fd);
    free(rightBitString);
	
}

int isLeaf(treeNode* tNode){
    if(tNode->left == NULL && tNode->right == NULL){
        return 1;
    } else {
        return 0;
    }
}
