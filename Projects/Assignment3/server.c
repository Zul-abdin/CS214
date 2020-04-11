#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct _fileNode_{
	char* filename;
	int filelength;
	struct _fileNode_* next;
	struct _fileNode_* prev;
}fileNode;


int setupServer(int socketfd, int port);
void unloadMemory();
void commandParser(int clientfd);
int bufferFill(int fd, char* buffer, int bytesToRead);
void commands(int clientfd);
char* doubleStringSize(char* word, int newsize);
int makeDirectory(char* directoryName);
void writeToFile(int fd, char* data);
void createProject(char* directoryName, int clientfd);
void readNbytes(int fd, int length,char* mode ,char* placeholder);
void createProject(char* directoryName, int clientfd);
void directoryTraverse(char* path, int mode);
void createManifest(int fd);

int main(int argc, char** argv){
	atexit(unloadMemory);
	if(argc != 2){
		printf("Fatal Error: invalid number of arguments\n");
	}else{
		//Assuming argv[1] is always a number currently
		int socketfd = socket(AF_INET, SOCK_STREAM, 0);
		if(socketfd == -1){
			printf("Fatal Error: Creating server socket failed\n");
		}else{
			if(setupServer(socketfd, atoi(argv[1])) == -1){
				printf("Fatal Error: Could not bind the server socket to the ip and port\n");
			}else{
				struct sockaddr_in client;
				socklen_t clientSize = sizeof(struct sockaddr_in);
				while(1){
					int clientfd = accept(socketfd, (struct sockaddr*) &client, &clientSize);
					if(clientfd == -1){
						printf("Error: Refused Connection\n");
					}else{
						printf("Successfully: Accepted Connection\n");
						commandParser(clientfd);
					}
				}
			}
		}
	}
	return 0;
}

void commandParser(int clientfd){
	char buffer[1] = {'\0'}; 
	int defaultSize = 15;
	char* token = malloc(sizeof(char) * (defaultSize + 1));
	memset(token, '\0', sizeof(char) * (defaultSize + 1));
	int read = 0;
	int bufferPos = 0;
	int tokenpos = 0;
	int numOfFiles = 0; //Could change this to array and have each index store the fileLength 
	int fileLength = 0;
	int filesRead = 0;
	int fileName = 0; //Could change this to a linked list of file names or an array of file names
	fileNode* listOfFiles = NULL; 
	char* mode = NULL;
	do{
		read = bufferFill(clientfd, buffer, sizeof(buffer));
		for(bufferPos = 0; bufferPos < (sizeof(buffer) / sizeof(buffer[0])); ++bufferPos){
			if(buffer[bufferPos] == '$'){
				if(mode == NULL){
					mode = token;
					printf("%s\n", mode);
				}else if(strcmp(mode, "create") == 0){
					readNbytes(clientfd, atoi(token), mode, NULL);
					free(token);
					break;	
				}else if(strcmp(mode, "sendFile") == 0){
					if(numOfFiles == 0){
						numOfFiles = atoi(token);
						listOfFiles = (fileNode*) malloc(sizeof(fileNode) * numOfFiles);
						free(token);
					}else if(fileName == 0){
						char* temp = NULL;
						readNbytes(clientfd, atoi(token), mode, temp);
						listOfFiles[filesRead].filename = temp;
						free(token);
						fileName = 1;
					}else{
						fileName = 0;
						listOfFiles[filesRead].filelength = atoi(token);
						filesRead++;
						free(token);
					}
					if(numOfFiles == filesRead){
						//LOOP THROUGH LINKED LIST OF FILE NODES and create and scan.
					}
				}
				defaultSize = 10;
				tokenpos = 0;
				token = (char*) malloc(sizeof(char) * (defaultSize + 1));
				memset(token, '\0', sizeof(char) * (defaultSize + 1));
			}else{
				if(tokenpos >= defaultSize){
					defaultSize = defaultSize * 2;
					token = doubleStringSize(token, defaultSize);
				}
				token[tokenpos] = buffer[bufferPos];
				tokenpos++;
			}
		}
	}while(buffer[0] != '\0' && read != 0);
	close(clientfd);
}

void readNbytes(int fd, int length, char* mode, char* placeholder){
	char buffer[100] = {'\0'};
	int defaultSize = 15;
	char* token = malloc(sizeof(char) * (defaultSize + 1));
	memset(token, '\0', sizeof(char) * (defaultSize + 1));
	int tokenpos = 0;
	int read = 0;
	int bufferPos = 0;

	do{
		if(length > sizeof(buffer)){
			read = bufferFill(fd, buffer, sizeof(buffer));
		}else{
			read = bufferFill(fd, buffer, length);
		}
		for(bufferPos = 0; bufferPos < read; ++bufferPos){
			if(tokenpos >= defaultSize){
				defaultSize = defaultSize * 2;
				token = doubleStringSize(token, defaultSize);
			}
			token[tokenpos] = buffer[bufferPos];
			tokenpos++;
		}
	}while(buffer[0] != '\0' && read != 0);
	if(strcmp("create", mode) == 0){
		createProject(token, fd);
		free(token);
	}
}

void createProject(char* directoryName, int clientfd){
	int success = makeDirectory(directoryName);
	if(success){
		char* manifest = malloc(sizeof(char) * strlen(directoryName) + 3 + strlen("/manifest"));
		memset(manifest, '\0', sizeof(char) * strlen(directoryName) + 3 + strlen("/manifest"));
		manifest[0] = '.';
		manifest[1] = '/';
		memcpy(manifest + 2, directoryName, strlen(directoryName));
		strcat(manifest, "/manifest");
		int fd = open(manifest, O_WRONLY | O_TRUNC | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
		createManifest(fd);
		close(fd);
		free(manifest);
	}else{
		//SEND ERROR TO CLIENT
	}
}
void createManifest(int fd){
	writeToFile(fd, "1");
	writeToFile(fd, "\n");
	writeToFile
}
void directoryTraverse(char* path, int mode){ 
	DIR* dirPath = opendir(path);
	if(!dirPath){
		printf("Fatal Error: Directory path does not exist!\n");
		return;
	}
	struct dirent* curFile = readdir(dirPath);
	while(curFile != NULL){
		if(strcmp(curFile->d_name, ".") == 0 || strcmp(curFile->d_name, "..") == 0){
			curFile = readdir(dirPath);
			continue;
		}
		if(curFile->d_type == DT_REG){
			//printf("File Found: %s\n", curFile->d_name);
			char* filepath = pathCreator(path, curFile->d_name);
			//printf("File path: %s\n", filepath);
			
			free(filepath);
		}else if(curFile->d_type == DT_DIR){
			char* directorypath = pathCreator(path, curFile->d_name);
			directoryTraverse(directorypath, mode);
		}else{
			
		}
		curFile = readdir(dirPath);
	}
	closedir(dirPath);
}

int makeDirectory(char* directoryName){
	int success = mkdir(directoryName, S_IRWXU);
	if(success == -1){
		printf("Warning: Directory could not be created\n");
		return 0;
	}else{
		printf("Directory Created\n");
		return 1;
	}
}

void writeToFile(int fd, char* data){
	int bytesToWrite = strlen(data);
	int bytesWritten = 0;
	int status = 0;
	while(bytesWritten < bytesToWrite){
		status = write(fd, (data + bytesWritten), (bytesToWrite - bytesWritten));
		if(status == -1){
			printf("Warning: write encountered an error\n");
			close(fd);
			return;
		}
		bytesWritten += status;
	}
}

char* doubleStringSize(char* word, int newsize){
	char* expanded =  (char*) malloc(sizeof(char) * (newsize + 1));
	memset(expanded, '\0', (newsize+1) * sizeof(char));
	memcpy(expanded, word, strlen(word));
	free(word);
	return expanded;
}

int bufferFill(int fd, char* buffer, int bytesToRead){
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
            return bytesRead;
        }
        bytesRead += status;
    }while(bytesRead < bytesToRead);
    	return bytesRead;
}


int setupServer(int socketfd, int port){
	struct sockaddr_in serverinfo;
	bzero((char*)&serverinfo, sizeof(serverinfo));
	serverinfo.sin_family = AF_INET;
	serverinfo.sin_port = htons(port);
	serverinfo.sin_addr.s_addr = INADDR_ANY;
	int success = bind(socketfd, (struct sockaddr*) &serverinfo, sizeof(struct sockaddr_in));
	if(success == -1){
		return -1;
	}else{
		listen(socketfd, 10);
		return 1;
	}
}

void unloadMemory(){
	printf("Deallocing structures and closing file descriptors\n");
}
