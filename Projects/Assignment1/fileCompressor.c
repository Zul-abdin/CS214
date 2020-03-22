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
	void* data;
	struct _NODE_* next;
	struct _NODE_* prev;
	int frequency;
}node;

typedef struct _hashtable_{
	node* element;
}hashtable;


void directoryTraverse(char* path, int recursive);
void bufferFill(int fd, char* buffer, int bytesToRead);
char* pathCreator(char* path, char* name);
int sizeHT = 500;

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
			printf("%s\n", curFile->d_name);
			char* filepath = pathCreator(path, curFile->d_name);
			printf("File path: %s\n", filepath);
			
		}else if(curFile->d_type == DT_DIR && recursive){
			printf("Directory Found\n%s\n", curFile->d_name);
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
	
	while(buffer != '\0'){
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
				free(word);
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

}
void insertHT(char* word){
	int index = getKey(word, sizeHT);
	node* newNode = (node *)malloc(sizeof(node) * 1);
	newNode->data = &word;
	if(hashT[index] != NULL){
	
	}else{
		hashT[index] = 
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
