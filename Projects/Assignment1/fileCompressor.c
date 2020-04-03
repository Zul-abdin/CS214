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
	char* bitstring;
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

typedef struct _HEAPNODE_{
    char* data;
    int frequency;
}heapNode;

typedef struct _AVLNODE_{
	char* data;
	int frequency;
	int height;
	struct _AVLNODE_* left;
	struct _AVLNODE_* right;
}avlnode;

// Globals
int itemCount = 0;
int invalidDirectory = 0;
int sizeHT = 100;
LLNode* head = NULL;
node** hashT = NULL;
char* escapeseq = NULL;
heapNode* heap;
int heapCapacity = 0;
int heapSize = 0;

//File Handling methods
void directoryTraverse(char* path, int recursive, int mode);
void fileReading(char* path, char* buffer, int bufferSize, int mode);
void bufferFill(int fd, char* buffer, int bytesToRead);
void writeToFile(char* word, int bytesToWrite, int whitespace, int fd);

//helper methods for file Processing
char* pathCreator(char* path, char* name);
int findBitstring(char* word, int fd);
char* doubleStringSize(char* word, int size);
char* getWhitespace(char*);

//Hashtable methods
void insertHT(char* word, int whitespace, int frequency, int rehashing, char* bitstring);
node* nodeInitialization(char* word, int whitespace, int frequency, char* bitstring);
int searchHT(char* word);
void printHT();
void rehash();
void freeNode(node* node);
int getKey(char* word, int size);

//Minheap & HuffmanCode methods
treeNode* createTreeNode(char* data, int frequency, treeNode* left, treeNode* right, int whitespace);
void initializeLL();
void LLSortedInsert(LLNode* newNode);
void processLL();
void processTree(treeNode* root, char* bitString, int fd);
void buildHuffman(char*);
char* sequenceReplace(char*);
int isLeaf(treeNode* tNode);
void freeHT(node** head);
void freeTree(treeNode* root);
void freeLLnode(LLNode* node);
int parentIndex(int pos);
int leftChildIndex(int pos);
int rightChildIndex(int pos);
int hasParent(int pos);
int hasLeftChild(int pos);
int hasRightChild(int pos);
void swap(int pos1, int pos2);
void checkHeapCapacity();
void siftUp();
void siftDown();
void heapInsert(heapNode newNode);
heapNode heapRemove();
heapNode* heapCreateNode(int frequency, char* data);


//Escape Sequence creation methods
void generateEscapeSeq();
int sequenceChecker(char* word);
char generatingchar();

int main(int argc, char** argv){
    heap = malloc(sizeof(heapNode) * 10);
    heapCapacity = 10;
	if(argc < 3 && argc > 5){
		printf("Fatal Error: too many or too few arguments\n");
		return 0;
	}
	hashT = (node**) calloc(sizeHT, sizeof(node*));	
	char buffer[100] = {'\0'};
	if(strlen(argv[1]) == 2 && argv[1][0] == '-' ){
		if(argv[1][1] == 'R'){
			if(strlen(argv[2]) == 2 && argv[2][0] == '-'){
				if(argv[2][1] == 'b' && argc == 4){ //build recursion
					char* _pathlocation_ = malloc(sizeof(char) * (strlen(argv[3]) + 1));
					memcpy(_pathlocation_, argv[3], strlen(argv[3]));
					_pathlocation_[strlen(argv[3])] = '\0';
					directoryTraverse(_pathlocation_, 1, 0);
					printHT();
					printf("%d\n", itemCount);

					if(invalidDirectory == 0){
						generateEscapeSeq();
						initializeLL();
						processLL();
						buildHuffman("HuffmanCodebook");
					}
				}else if(argv[2][1] == 'c' && argc == 5){ //compress recursion		
					fileReading(argv[4],buffer,sizeof(buffer), 3);
					printHT();
					printf("%d\n", itemCount);
					if(invalidDirectory == 0){
						char* _pathlocation_ = malloc(sizeof(char) * (strlen(argv[3]) + 1));
						memcpy(_pathlocation_, argv[3], strlen(argv[3]));
						_pathlocation_[strlen(argv[3])] = '\0';
						directoryTraverse(_pathlocation_, 1, 1);
					}		
				}else if(argv[2][1] == 'd' && argc == 5){ //decompress recursion

					fileReading(argv[4],buffer,sizeof(buffer), 4);
					printHT();
					printf("%d\n", itemCount);
							
					if(invalidDirectory == 0){
						char* _pathlocation_ = malloc(sizeof(char) * (strlen(argv[3]) + 1));
						memcpy(_pathlocation_, argv[3], strlen(argv[3]));
						_pathlocation_[strlen(argv[3])] = '\0';
						directoryTraverse(_pathlocation_, 1, 2);
					}
							
				}else{
					printf("Fatal Error: Wrong Arguments\n");
				}
			}else{
				printf("Fatal Error: Wrong Arguments\n");
			}
		}else if(argv[1][1] == 'b' && argc == 3){ //build mode no recursion
				char* _pathlocation_ = malloc(sizeof(char) * (strlen(argv[2]) + 1));
				memcpy(_pathlocation_, argv[2], strlen(argv[2]));
				_pathlocation_[strlen(argv[2])] = '\0';
				directoryTraverse(_pathlocation_, 0, 0);
				printHT();
				printf("%d\n", itemCount);
				if(invalidDirectory == 0){
					generateEscapeSeq();
					initializeLL();
					processLL();
					buildHuffman("HuffmanCodebook");
				}
		}else if(argv[1][1] == 'c' && argc == 4){ //compress no recursion
			fileReading(argv[3],buffer,sizeof(buffer), 3);
			printHT();
			printf("%d\n", itemCount);	
			if(invalidDirectory == 0){
				char* _pathlocation_ = malloc(sizeof(char) * (strlen(argv[2]) + 1));
				memcpy(_pathlocation_, argv[2], strlen(argv[2]));
				_pathlocation_[strlen(argv[2])] = '\0';	
				directoryTraverse(_pathlocation_, 0, 1);
			}
				
		}else if(argv[1][1] == 'd' && argc == 4){ //decompress no recursion
			fileReading(argv[3],buffer,sizeof(buffer), 4);
			printHT();
			printf("%d\n", itemCount);	
			if(invalidDirectory == 0){
				char* _pathlocation_ = malloc(sizeof(char) * (strlen(argv[2]) + 1));
				memcpy(_pathlocation_, argv[2], strlen(argv[2]));
				_pathlocation_[strlen(argv[2])] = '\0';
				directoryTraverse(_pathlocation_, 0, 2);
			}
				
		}else{
			printf("Fatal Error: Wrong Arguments\n");
		}
	}else{
		printf("Fatal Error: Wrong Arguments\n");
	}
	freeHT(hashT);
	while(head != NULL){
		LLNode* temp = head;
		head = head->next;
		freeLLnode(temp);
	}
	if(escapeseq != NULL){
		free(escapeseq);
	}
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
		invalidDirectory = 1;
		return;
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
			int compressFile = 0;
			if(strlen(curFile->d_name) > 7 && strcmp((curFile->d_name + (strlen(curFile->d_name) - 3)), "hcz") == 0){
				printf("Compress file found\n");
				compressFile = 1;
			}
			if(mode == 0 && compressFile == 0){
				printf("Building the HufmanCodebook\n");
				char buffer[100] = {'\0'};
				fileReading(filepath, buffer, sizeof(buffer), mode);
			}else if(mode == 1 && compressFile == 0){
				printf("Compressing the file\n");
				char buffer[100] = {'\0'};
				fileReading(filepath, buffer, sizeof(buffer), mode);
			}else if(mode == 2 && compressFile){\
				printf("Decompressing the file\n");
				char buffer[100] = {'\0'};
				fileReading(filepath, buffer, sizeof(buffer), mode);
			}
			free(filepath);
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
	newpath[strlen(name) + strlen(path) + 1] = '\0';
	return newpath;
}
/*
	fileReading modes:
	0 -> build
	1 -> compress
	2 -> decompress
	3 -> compression huffman code processing (hashtable is sorted by word)
	4 -> decompression huffman code processing (hashtable is sorted by bitstring)
*/
void fileReading(char* path, char* buffer, int bufferSize, int mode){ 
	int fd = open(path, O_RDONLY);
	int fw = -2;
	char* filename = NULL;
	if(mode == 1){
		filename = (char*) malloc((strlen(path) + 5) * sizeof(char));
		memset(filename, '\0', (strlen(path) + 5) * sizeof(char));
		memcpy(filename, path, strlen(path));
		strcat(filename, ".hcz");
		fw = open(filename, O_WRONLY | O_TRUNC | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
		printf("filename:%s\n", filename);
		free(filename);
	}else if(mode == 2){
		filename = (char*) malloc((strlen(path)- 3) * sizeof(char));
		memset(filename, '\0', (strlen(path) - 3) * sizeof(char));
		memcpy(filename, path, strlen(path) - 4);
		fw = open(filename, O_WRONLY | O_TRUNC | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
		printf("filename:%s\n", filename);
		free(filename);
	}
	
	if(fd == -1 || fw == -1){
		if(fd != -1){
			close(fd);
		}
		if(fw >= 0){
			close(fw);
		}
		printf("Fatal Error: File does not exist\n");
		invalidDirectory = 1;
		return;
	}
	bufferFill(fd, buffer, bufferSize);
	
	int bufferPos;
	int defaultSize = 20;
	char* word = (char*) malloc(sizeof(char) * defaultSize);
	word = memset(word, '\0', sizeof(char) * defaultSize);
	int wordpos = 0;
	char* bitstring = NULL;
	int tabCheck = 1;
	int escseq = 0;
	if(mode == 3 || mode == 4){
		bitstring = (char*) malloc(sizeof(char) * defaultSize);
		bitstring = memset(bitstring, '\0', sizeof(char) * defaultSize);
		tabCheck = 0;
	}
	
	while(buffer[0] != '\0'){	
		for(bufferPos = 0; bufferPos < (bufferSize/sizeof(buffer[0])); ++bufferPos){
			if(mode == 2){
				if(buffer[bufferPos] == '\n'){
					printf("Last token processed\n");
				}else{
					if(wordpos >= defaultSize){
						defaultSize = defaultSize * 2;
						word = doubleStringSize(word, defaultSize);
					}
					word[wordpos] = buffer[bufferPos];
					wordpos++;
					if(findBitstring(word, fw) == 1){
						wordpos = 0;
						free(word);
						defaultSize = 20;
						word = (char *)malloc(sizeof(char) * (defaultSize + 1));
						memset(word, '\0', (sizeof(char) * (defaultSize + 1)));
					}
				}
			}else {
				if(iscntrl(buffer[bufferPos]) != 0 || buffer[bufferPos] == ' '){
					if(mode == 0){
						if(itemCount == sizeHT){
							rehash();
						}
						char* whitespaceholder = (char*) malloc(sizeof(char) * (1 + 1));
						memset(whitespaceholder, '\0', (sizeof(char) * (1 + 1)));
						memcpy(whitespaceholder, (buffer + bufferPos), sizeof(char));
					
						insertHT(whitespaceholder, 1, 1, 0, bitstring);
						if(word[0] != '\0'){
							insertHT(word, 0, 1, 0, bitstring);
						}else{
							free(word);
						}
						
						defaultSize = 20;
						word = (char *)malloc(sizeof(char) * (defaultSize + 1));
						memset(word, '\0', (sizeof(char) * (defaultSize + 1)));

					}else if(mode == 3 || mode == 4){
						if(buffer[bufferPos] == '\t'){
							tabCheck = 1;
						}else if(buffer[bufferPos] == '\n'){
							if(escseq == 0 && tabCheck == 0){
								escseq = 1;
								escapeseq = bitstring;
								
								defaultSize = 20;
								bitstring = (char *)malloc(sizeof(char) * (defaultSize + 1));
								memset(bitstring, '\0', (sizeof(char) * (defaultSize + 1)));
								
							}else{
								if(itemCount == sizeHT){
									rehash();
								}
								if(strlen(word) >= (strlen(escapeseq)) && memcmp(word, escapeseq, strlen(escapeseq)) == 0){
									word = getWhitespace(word);
								}
								if(mode == 4){
									insertHT(bitstring, 0, 1, 0, word);
								}else{
									insertHT(word, 0, 1, 0, bitstring);
								}
								tabCheck = 0;
								
								defaultSize = 20;
								word = (char *)malloc(sizeof(char) * (defaultSize + 1));
								memset(word, '\0', (sizeof(char) * (defaultSize + 1)));
								bitstring = (char*) malloc(sizeof(char) * (defaultSize + 1));
								memset(bitstring, '\0', (sizeof(char) * (defaultSize + 1)));
							}
							
						}else{
							//printf("Warning: Huffman Codebook not formatted correctly\n");
						}
					}else if(mode == 1){
						findBitstring(word, fw);
						char* temp = malloc(sizeof(char) * 1);
						temp[0] = buffer[bufferPos];
						findBitstring(temp, fw);
						free(word);
						free(temp);
						defaultSize = 20;
						word = (char *)malloc(sizeof(char) * (defaultSize + 1));
						memset(word, '\0', (sizeof(char) * (defaultSize + 1)));
					}
					
					defaultSize = 20;
					wordpos = 0;
					
				}else{
					if(wordpos >= defaultSize){
						if(tabCheck){
							defaultSize = defaultSize * 2;
						  	word = doubleStringSize(word, defaultSize);
						}else{
						 	defaultSize = defaultSize * 2;
						  	bitstring = doubleStringSize(bitstring, defaultSize);
					  	}
			  	  	}
			  	  	if(tabCheck){
			  	  		word[wordpos] = buffer[bufferPos];
			  	  	}else{
			  	  		bitstring[wordpos] = buffer[bufferPos];
			  	  	}
			  	  	wordpos++;
				}
			}
		}
		bufferFill(fd, buffer, bufferSize);	
	}
	if(mode == 0 && word[0] != '\0'){ //Gets the last token if it does not end with a delimiter (treats EOF as delimiter) 
		insertHT(word, 0, 1, 0, bitstring);
	}else if(mode == 1 && word[0] != '\0'){
		findBitstring(word, fw);
		free(word);
	}else if(mode == 2 && word[0] != '\0'){
		printf("Warning: Compress File formatted improperly\n");
		free(word);
	}else{
		free(word);
		if(mode == 3 || mode ==  4){
			free(bitstring);
		}
	}
	if(close(fd) < 0){
		printf("Warning: File Descriptor would not close\n");
	}
	if((mode == 1 || mode == 2) && close(fw) < 0){
		printf("Warning: File Descriptor would not close\n");
	}
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

void writeToFile(char* word, int bytesToWrite, int delimiter, int fd){
	int bytesWritten = 0;
	int status = 0;
	if(delimiter){ 
		writeToFile(escapeseq, strlen(escapeseq), 0, fd);
		word = sequenceReplace(word); 
		bytesToWrite = strlen(word);
	}
	while(bytesWritten < bytesToWrite){
		status = write(fd, (word + bytesWritten), (bytesToWrite - bytesWritten));
		if(status == -1){
			printf("Warning: write encountered an error\n");
			return;
		}
		bytesWritten += status;
	}
	if(delimiter){
		free(word);
	}
}

void generateEscapeSeq(){
	int validEscapeSeq = 0;
	int defaultSize = 2;
	do{
		char* oldseq = escapeseq;
		escapeseq = (char*) malloc(sizeof(char) * defaultSize + 1);
		memset(escapeseq, '\0', (sizeof(char) * defaultSize + 1));
		if(oldseq != NULL){
			memcpy(escapeseq, oldseq, strlen(oldseq) * sizeof(char));
			free(oldseq);
		}
		char temp = generatingchar();
		escapeseq[defaultSize - 2] = temp;
		defaultSize = defaultSize + 1;
		if(sequenceChecker(escapeseq) == -1){
			validEscapeSeq = 1;
		}
	}while(validEscapeSeq == 0);
}

int sequenceChecker(char* word){
	node* curNode = NULL;
	int i = 0;
	for(i = 0; i < sizeHT; ++i){
		curNode = hashT[i];
		while(curNode != NULL){
			if(strlen(curNode->data) >= strlen(word)){
				if(memcmp(curNode->data, word, strlen(word)) == 0){
					return 1;
				}
			}
			curNode = curNode->next;
		}
	}
	return -1;
}

char generatingchar(){
	int r = rand() % 6;
	switch(r){
		case 0: return '$';
		case 1: return '&';
		case 2: return '*';
		case 3: return '#';
		case 4: return '!';
		case 5: return '~';
		default: return '_';
	}
}

char* doubleStringSize(char* word, int newsize){
	char* expanded =  (char*) malloc(sizeof(char) * (newsize + 1));
	memset(expanded, '\0', (newsize+1) * sizeof(char));
	memcpy(expanded, word, strlen(word));
	free(word);
	return expanded;
}

char* getWhitespace(char* word){
	char* code = (word + strlen(escapeseq));
	int letter = atoi(code);
	char* temp = malloc(sizeof(char) * 2);
	memset(temp, '\0', (sizeof(char) * 2));
	switch(letter){
		case 0: temp[0] = 0;
					break;
		case 1: temp[0] = 1;
					break;
		case 2: temp[0] = 2;
					break;
		case 3: temp[0] = 3;
					break;
		case 4: temp[0] = 4;
					break;
		case 5: temp[0] = 5;
					break;
		case 6: temp[0] = 6;
					break;
		case 7: temp[0] = 7;
					break;
		case 8: temp[0] = 8;
					break;
		case 9: temp[0] = 9;
					break;
		case 10: temp[0] = 10;
					break;
		case 11: temp[0] = 11;
					break;
		case 12: temp[0] = 12;
					break;
		case 13: temp[0] = 13;
					break;
		case 14: temp[0] = 14;
					break;
		case 15: temp[0] = 15;
					break;
		case 16: temp[0] = 16;
					break;
		case 17: temp[0] = 17;
					break;
		case 18: temp[0] = 18;
					break;
		case 19: temp[0] = 19;
					break;
		case 20: temp[0] = 20;
					break;
		case 21: temp[0] = 21;
					break;
		case 22: temp[0] = 22;
					break;
		case 23: temp[0] = 23;
					break;
		case 24: temp[0] = 24;
					break;
		case 25: temp[0] = 25;
					break;
		case 26: temp[0] = 26;
					break;
		case 27: temp[0] = 27;
					break;
		case 28: temp[0] = 28;
					break;
		case 29: temp[0] = 29;
					break;
		case 30: temp[0] = 30;
					break;
		case 31: temp[0] = 31;
					break;
		case 32: temp[0] = ' ';
					break;
		default: printf("Warning: escape sequence detected but not valid control code\n");
					break;
	}
	free(word);
	return temp;
}

int findBitstring(char* word, int fd){
	int index = getKey(word, sizeHT);
	node* curNode = hashT[index];
	while(curNode != NULL){
		if(strcmp(curNode->data, word) == 0){
			writeToFile(curNode->bitstring, strlen(curNode->bitstring), 0, fd);
			return 1;
		}
		curNode = curNode->next;
	}
	return -1;
}

int getKey(char* word, int size){
	int loop = 0;
	int hash = 0;
	for(loop = 0; loop < strlen(word); ++loop){
		hash = ((hash << 5) - hash) + word[loop];
	}
	int index = hash % size;
	if(index < 0){
		//printf("Negative Index: %s\n", word);
		index *= -1;
	}
	return index;
}

void insertHT(char* word, int whitespace, int frequency, int rehashing, char* bitstring){
	int index = getKey(word, sizeHT);
	node* newNode = nodeInitialization(word, whitespace, frequency, bitstring);
	if(hashT[index] == NULL){
		hashT[index] = newNode;
	}else{
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
	}
	if(rehashing == 0){
		itemCount++;
	}
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
			insertHT(curNode->data, curNode->whitespace, curNode->frequency, 1, curNode->bitstring);
			node* temp = curNode;
			curNode = curNode->next;
			free(temp);
		}
	}
	free(oldHead);
}

node* nodeInitialization(char* word, int whitespace, int frequency, char* bitstring){
	node* newNode = (node *) malloc(sizeof(node) * 1);
	
	newNode->data = word;
	newNode->frequency = frequency;
	newNode->whitespace = whitespace;
	newNode->bitstring = bitstring;
	
	newNode->next = NULL;
	newNode->prev = NULL;
	
	printf("%s\n", newNode->data); 
	
	return newNode;
}

void printHT(){
	int i = 0;
	for(i = 0; i < sizeHT; i++){
	   node* curr = hashT[i];
		while(curr != NULL){
				printf("%s with frequency %d and bitstring %s\n", curr->data, curr->frequency, curr->bitstring);
            curr = curr->next;
		}
	}
}

int searchHT(char* word){
	int index = getKey(word, sizeHT);
	node* curNode = hashT[index];
	while(curNode != NULL){
		if(strcmp(curNode->data, word) == 0){
			return 1;
		}
		curNode = curNode->next;
	}
	return -1;
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
	free(curNode->bitstring);
	free(curNode->data);
	free(curNode);
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

void processTree(treeNode* root, char* bitString, int fd){
    if(root == NULL){
        return;
    }
	 if(isLeaf(root)){
        printf("%s\t%s\n", root->data, bitString);
        writeToFile(bitString, strlen(bitString), 0, fd);
        writeToFile("\t", strlen("\t"), 0, fd);
		  writeToFile(root->data, strlen(root->data), root->whitespace, fd);
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

void buildHuffman(char* filename){
	int fd = open(filename, O_WRONLY | O_TRUNC | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
	writeToFile(escapeseq, strlen(escapeseq), 0, fd);
	writeToFile("\n", strlen("\n"), 0, fd);
	processTree(head->data, "", fd);
	close(fd);
}

char* sequenceReplace(char* word){
	char* temp = malloc(sizeof(char) * 3);
	memset(temp, '\0', (sizeof(char) * 3));
	char temp1 = word[0];
	switch(temp1){
		case 0: memcpy(temp, "0", strlen("0"));
					break;
		case 1: memcpy(temp, "1", strlen("1"));
					break;
		case 2: memcpy(temp, "2", strlen("2"));
					break;
		case 3: memcpy(temp, "3", strlen("3"));
					break;
		case 4: memcpy(temp, "4", strlen("4"));
					break;
		case 5: memcpy(temp, "5", strlen("5"));
					break;
		case 6: memcpy(temp, "6", strlen("6"));
					break;
		case 7: memcpy(temp, "7", strlen("7"));
					break;
		case 8: memcpy(temp, "8", strlen("8"));
					break;
		case 9: memcpy(temp, "9", strlen("9"));
					break;
		case 10: memcpy(temp, "10", strlen("10"));
					break;
		case 11: memcpy(temp, "11", strlen("11"));
					break;
		case 12: memcpy(temp, "12", strlen("12"));
					break;
		case 13: memcpy(temp, "13", strlen("13"));
					break;
		case 14: memcpy(temp, "14", strlen("14"));
					break;
		case 15: memcpy(temp, "15", strlen("15"));
					break;
		case 16: memcpy(temp, "16", strlen("16"));
					break;
		case 17: memcpy(temp, "17", strlen("17"));
					break;
		case 18: memcpy(temp, "18", strlen("18"));
					break;
		case 19: memcpy(temp, "19", strlen("19"));
					break;
		case 20: memcpy(temp, "20", strlen("20"));
					break;
		case 21: memcpy(temp, "21", strlen("21"));
					break;
		case 22: memcpy(temp, "22", strlen("22"));
					break;
		case 23: memcpy(temp, "23", strlen("23"));
					break;
		case 24: memcpy(temp, "24", strlen("24"));
					break;
		case 25: memcpy(temp, "25", strlen("25"));
					break;
		case 26: memcpy(temp, "26", strlen("26"));
					break;
		case 27: memcpy(temp, "27", strlen("27"));
					break;
		case 28: memcpy(temp, "28", strlen("28"));
					break;
		case 29: memcpy(temp, "29", strlen("29"));
					break;
		case 30: memcpy(temp, "30", strlen("30"));
					break;
		case 31: memcpy(temp, "31", strlen("31"));
					break;
		case ' ': memcpy(temp, "32", strlen("32"));
					break;
		default: printf("Warning: delimiter detected but could not return the character presentation\n");
					break;
	}
	free(word);
	return temp;
}

void freeLLnode(LLNode* node){
	freeTree(node->data);
	free(node);
}

void freeTree(treeNode* root){	
	free(root->data);
	if(root->left != NULL){
		freeTree(root->left);
	}
	if(root->right != NULL){
		freeTree(root->right);
	}
	free(root);
}

int parentIndex(int pos){
    return (pos-1) / 2;
}

int leftChildIndex(int pos){
    return (2 * pos) + 1;
}

int rightChildIndex(int pos){
    return (2 * pos) + 2;
}

int hasLeftChild(int pos){
    if(leftChildIndex(pos) < heapSize){
        return 1;
    } else {
        return 0;
    }
}
int hasRightChild(int pos){
    if(rightChildIndex(pos) < heapSize){
        return 1;
    } else {
        return 0;
    }
}

int hasParent(int pos){
    if(parentIndex(pos) >= 0){
        return 1;
    } else {
        return 0;
    }
}

void swap(int pos1, int pos2){
    heapNode temp = heap[pos1];
    heap[pos1] = heap[pos2];
    heap[pos2] = temp;
}

void checkHeapCapacity(){
    if(heapSize == heapCapacity){
        heapNode* temp = malloc(sizeof(heapNode) * heapCapacity * 2);
        memcpy(temp, heap, heapCapacity);
        free(heap);
        heap = temp;
        heapCapacity *= 2;
    }
}

void siftUp(){
    int index = heapSize - 1;
    while(hasParent(index) && heap[parentIndex(index)].frequency > heap[index].frequency){
        swap(parentIndex(index), index);
        index = parentIndex(index);
    }
}

void siftDown(){
    int index = 0;
    while(hasLeftChild(index)){
        int lesserChild = leftChildIndex(index);
        if(hasRightChild(index) && heap[rightChildIndex(index)].frequency < heap[leftChildIndex(index)].frequency){
            lesserChild = rightChildIndex(index);
        }
        if(heap[index].frequency < heap[lesserChild].frequency){
            break;
        } else {
            swap(index, lesserChild);
        }
        index = lesserChild;
    }
}

void heapInsert(heapNode newNode){
    checkHeapCapacity();
    heap[heapSize] = newNode;
    heapSize++;
    siftUp();
}

heapNode heapRemove(){
    heapNode oldRoot = heap[0];
    heap[0] = heap[heapSize - 1];
    heapSize--;
    siftDown();
    return oldRoot;
}

heapNode* heapCreateNode(int frequency, char* data){
    heapNode* newNode = malloc(sizeof(heapNode));
    newNode->frequency = frequency;
    newNode->data = data;
    return newNode;
}
