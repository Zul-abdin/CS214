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

int itemCount = 0;
int sizeHT = 100;
node** hashT = NULL;
char* escapeseq = NULL;
LLNode* head = NULL;
int invalidDirectory = 0;
void directoryTraverse(char* path, int recursive, int mode);
void bufferFill(int fd, char* buffer, int bytesToRead);
void fileReading(char* path, char* buffer, int bufferSize, int mode);
char* pathCreator(char* path, char* name);
void freeNode(node* node);
void insertHT(char* word, int whitespace, int frequency, int rehashing, char* bitstring);
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
void compress(char* huffmancodebook);
char* doubleStringSize(char* word, int size);
char* getWhitespace(char*);
int findBitstring(char* word, int fd);
char generatingchar();
void generateEscapeSeq();
int searchHT(char* word);
int sequenceChecker(char* word, int wordlength);

int main(int argc, char** argv){
	if(argc < 3 && argc > 5){
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
							printHT();
							printf("%d\n", itemCount);

							if(invalidDirectory == 0){
								generateEscapeSeq();
								initializeLL();
								processLL();

								fileWriting("HuffmanCodebook.txt");
							}
						}else if(argv[2][1] == 'c' && argc == 5){ //compress recursion
							hashT = (node**) calloc(sizeHT, sizeof(node*));				
				
							char buffer[100] = {'\0'};
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
							hashT = (node**) calloc(sizeHT, sizeof(node*));	
							char buffer[100] = {'\0'};
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
			}else if(argv[1][1] == 'b' && argc == 3){ //build mode no recursion
					hashT = (node**) calloc(sizeHT, sizeof(node*));
					char* _pathlocation_ = malloc(sizeof(char) * (strlen(argv[2]) + 1));
					memcpy(_pathlocation_, argv[2], strlen(argv[2]));
					_pathlocation_[strlen(argv[2])] = '\0';
					directoryTraverse(_pathlocation_, 0, 0);
					printHT();
					printf("%d\n", itemCount);
					if(invalidDirectory == 0){
						generateEscapeSeq();
						printf("Writing to file\n");
						initializeLL();
						processLL();

						fileWriting("HuffmanCodebook.txt");
					}

			}else if(argv[1][1] == 'c' && argc == 4){ //compress no recursion
				hashT = (node**) calloc(sizeHT, sizeof(node*));				
				
				char buffer[100] = {'\0'};
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
				hashT = (node**) calloc(sizeHT, sizeof(node*));	
				char buffer[100] = {'\0'};
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
				return 0;
			}
	}else{
		printf("Fatal Error: Wrong Arguments\n");
		return 0;
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
void fileReading(char* path, char* buffer, int bufferSize, int mode){ //Old Template: void fileReading(char* path, char* buffer, char* delimiters, int bufferSize, int delimiterSize);
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
		printf("Error: File does not exist: should not be possible\n");
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
				if(isspace(buffer[bufferPos])){
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
								if(strlen(word) == (strlen(escapeseq) + 1) && memcmp(word, escapeseq, strlen(escapeseq)) == 0){
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
							printf("Warning: Huffman Codebook not formatted correctly\n");
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
		printf("Error: Compress File formatted improperly\n");
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
	if(mode == 1 && close(fw) < 0){
		printf("Warning: File Descriptor would not close\n");
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
		if(sequenceChecker(escapeseq, defaultSize) == -1){
			validEscapeSeq = 1;
		}
	}while(validEscapeSeq == 0);
}

int sequenceChecker(char* word, int wordlength){
	if(searchHT(word) == 1){
		return 1;
	}
	int i;
	char ws[] = {'n','s','t','v','f','r'};
	for(i = 0; i < 6; ++i){
		word[wordlength - 2] = ws[i];
		if(searchHT(word) == 1){
			word[wordlength - 2] = '\0';
			return 1;
		}
	}
	word[wordlength - 2] = '\0';
	return -1;
}
int searchHT(char* word){
	int index = getKey(word, sizeHT);
	if(hashT[index] != NULL){
		node* curNode = hashT[index];
		while(curNode != NULL){
			if(strcmp(curNode->data, word) == 0){
				return 1;
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
	printf("Not found in the list of Huffman Codes\n");
}
char* getWhitespace(char* word){
	char letter = word[strlen(word) - 1];
	char* temp = malloc(sizeof(char) * 2);
	memset(temp, '\0', (sizeof(char) * 2));
	switch(letter){
		case 'n': temp[0] = '\n';
					break;
		case 's': temp[0] = ' ';
					break;
		case 't': temp[0] = '\t';
					break;
		case 'v': temp[0] = '\v';
					break;
		case 'f': temp[0] = '\f';
					break;
		case 'r': temp[0] = '\r';
					break;
		default: printf("ERROR: escape seq detected but not valid whitespace\n");
					break;
	}
	free(word);
	return temp;
}

char* doubleStringSize(char* word, int size){
	char* expanded =  (char*) malloc(sizeof(char) * (size + 1));
	memset(expanded, '\0', (size+1) * sizeof(char));
	memcpy(expanded, word, strlen(word));
	free(word);
	return expanded;
}

int getKey(char* word, int size){
	int loop = 0;
	int hash = 0;
	for(loop = 0; loop < strlen(word); ++loop){
		hash = ((hash << 5) - hash) + word[loop];
	}
	int index = hash % size;
	if(index < 0){
		printf("Negative Index: %s\n", word);
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
			insertHT(curNode->data, curNode->whitespace, curNode->frequency, 1, curNode->bitstring);
			node* temp = curNode;
			curNode = curNode->next;
			free(temp);
		}
	}
	free(oldHead);
}

void insertHT(char* word, int whitespace, int frequency, int rehashing, char* bitstring){
	int index = getKey(word, sizeHT);
	node* newNode = (node *)malloc(sizeof(node) * 1);
	newNode->data = word;
	printf("%s\n", newNode->data);
	newNode->next = NULL;
	newNode->prev = NULL;
	newNode->frequency = frequency;
	newNode->whitespace = whitespace;
	newNode->bitstring = bitstring;
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
				printf("%s with frequency %d and bitstring %s\n", curr->data, curr->frequency, curr->bitstring);
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
	free(curNode->bitstring);
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
	char* temp = malloc(sizeof(char) * 2);
	memset(temp, '\0', (sizeof(char) * 2));
	char temp1 = word[0];
	switch(temp1){
		case '\n': temp[0] = 'n';
					break;
		case ' ': temp[0] = 's';
					break;
		case '\t': temp[0] = 't';
					break;
		case '\v': temp[0] = 'v';
					break;
		case '\f': temp[0] = 'f';
					break;
		case '\r': temp[0] = 'r';
					break;
		default: printf("ERROR: whitespace is not actually a whitespace character\n");
					break;
	}
	free(word);
	return temp;
}
void fileWriting(char* filename){
	int fd = open(filename, O_WRONLY | O_TRUNC | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
	writeToFile(escapeseq, strlen(escapeseq), 0, fd);
	writeToFile("\n", strlen("\n"), 0, fd);
	processTree(head->data, "", fd);
	close(fd);
}
void writeToFile(char* word, int bytesToWrite, int whitespace, int fd){
	int bytesWritten = 0;
	int status = 0;
	if(whitespace){ //This case: bytesToWrite should be 1 so we do not need to change it when we turn whitespace to its counterpart
		writeToFile(escapeseq, strlen(escapeseq), 0, fd);
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
	if(whitespace){
		free(word);
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
