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

int itemCount = 0;
int sizeHT = 1000;
node* hashT[1000] = {NULL};

void directoryTraverse(char* path, int recursive);
void bufferFill(int fd, char* buffer, int bytesToRead);
void readingFile(char* path, char* buffer, char* delimiters, int bufferSize, int delimiterSize);
char* pathCreator(char* path, char* name);
void freeNode(node* node);
void insertHT(char* word);
void printHT();

int main(int argc, char** argv){
//Assuming third argument is filepath atm, simple test
	if(argc != 3 && argc != 4){
		printf("Fatal Error: Not Enough Arguments\n");
		return 0;
	}
	char* _pathlocation_ = malloc(sizeof(char) * (strlen(argv[2]) + 1));
	memcpy(_pathlocation_, argv[2], strlen(argv[2]));
	_pathlocation_[strlen(argv[2])] = '\0';
	directoryTraverse(_pathlocation_, 1);
	
	printHT();
	return 0;
}

void directoryTraverse(char* path, int recursive){
	DIR* dirPath = opendir(path);
	if(!dirPath){
		printf("Fatal Error: Directory path does not exist!\n");
		closedir(dirPath);
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
			
			char delimiters[2] = {' ', '\n'};
			char buffer[100] = {'\0'};
			readingFile(filepath, buffer, delimiters, sizeof(buffer), sizeof(delimiters));
			
		}else if(curFile->d_type == DT_DIR && recursive){
			printf("Directory Found: %s\n", curFile->d_name);
			char* directorypath = pathCreator(path, curFile->d_name);
			printf("Directory Location: %s\n", directorypath);
			directoryTraverse(directorypath, recursive);
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

void readingFile(char* path, char* buffer, char* delimiters, int bufferSize, int delimiterSize){
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
			int delimiterPos;
			int isDelimiter = 0;
			for(delimiterPos = 0; delimiterPos < (delimiterSize/sizeof(delimiters[0])); ++delimiterPos){
				if(buffer[bufferPos] == delimiters[delimiterPos]){
					isDelimiter = 1;
					break;
				}
			}
			
			if(isDelimiter){
				insertHT(word);
				
				defaultSize = 20;
				wordpos = 0;
				word = (char *)malloc(sizeof(char) * defaultSize);
				
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
}

void printHT(){
	int i = 0;
	for(i = 0; i < sizeHT; i++){
		if(hashT[i] != NULL){
			while(hashT[i] != NULL){
				printf("%s with frequency %d\n", hashT[i]->data, hashT[i]->frequency);
				hashT[i] = hashT[i]->next;
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
