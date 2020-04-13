#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/socket.h>
#include <netdb.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>

typedef struct _fileNode_{
	char* filename;
	char* filepath;
	long long filelength;
	struct _fileNode_* next;
	struct _fileNode_* prev;
}fileNode;

void writeToFile(int fd, char* data);
char* doubleStringSize(char* word, int newsize);
char** getConfig();
int bufferFill(int fd, char* buffer, int bytesToRead);
int connectToServer(int socketfd, char** serverinfo);
int createSocket();
int setupConnection();
void metadataParser(int clientfd);
void readNbytes(int fd, int length, char* mode, char** placeholder);
void writeToFileFromSocket(int socketfd, fileNode* files);
void writeToSocketFromFile(int clientfd, char* fileName);
void createProject(char* directoryName, int socketfd);
int makeDirectory(char* directoryName);

fileNode* listOfFiles;

int main(int argc, char** argv) {
    if(argc != 4 && argc != 3){
    	printf("Incorrect number of arguments.\n");    
    }else{
    	if(argc == 3){
    		if(strlen(argv[1]) == 8 && strcmp(argv[1], "checkout") == 0){  //checkout
    			
    		}else if(strlen(argv[1]) == 6 && strcmp(argv[1], "update") == 0){ //update
    			
    		}else if(strlen(argv[1]) == 7 && strcmp(argv[1], "upgrade") == 0){ //upgrade
    			
    		}else if(strlen(argv[1]) == 6 && strcmp(argv[1], "commit") == 0){ //commit
    			
    		}else if(strlen(argv[1]) == 4 && strcmp(argv[1], "push") == 0){ //push
    			
    		}else if(strlen(argv[1]) == 6 && strcmp(argv[1], "create") == 0){ //create
				int socketfd = setupConnection();
				if(socketfd > 0){
					writeToFile(socketfd, "create$");
					char str[3] = {'\0'};
					sprintf(str, "%lu", strlen(argv[2]));
					writeToFile(socketfd, str);
					writeToFile(socketfd, "$");
					writeToFile(socketfd, argv[2]);
					char* temp = NULL;
					readNbytes(socketfd, strlen("FAILURE"), NULL, &temp);
					if(strcmp(temp, "SUCCESS") == 0){
						printf("Server sucessfully created the project, recieving the Manifest\n");
						metadataParser(socketfd);
						createProject(argv[2], socketfd);
					}else if(strcmp(temp , "FAILURE") == 0){
						printf("Server failed to created the project\n");
					}
					close(socketfd);
				}else{
				
				}
    		}else if(strlen(argv[1]) == 7 && strcmp(argv[1], "destroy") == 0){ //destroy
    			
    		}else if(strlen(argv[1]) == 14 && strcmp(argv[1], "currentversion") == 0){ //currentversion
				int socketfd = setupConnection();
				if(socketfd > 0){
                    writeToFile(socketfd, "currentversion$");
                    char str[3] = {'\0'};
                    sprintf(str, "%lu", strlen(argv[2]));
                    writeToFile(socketfd, str);
                    writeToFile(socketfd, "$");
                    writeToFile(socketfd, argv[2]);
                    char* temp = NULL;
                    do {
                        readNbytes(socketfd, 50, NULL, &temp);
                        printf("%s", temp);
                        readNbytes(socketfd, 1, NULL, &temp);
                    } while(strcmp(temp, "$") == 0);
                    close(socketfd);
				}else{
				
				}
    		}else if(strlen(argv[1]) == 7 && strcmp(argv[1], "history") == 0){ //history
    			
    		}else{
    			printf("Invalid operation\n");
    		}
    		
    	}else{
    		if(strlen(argv[1]) == 9 && strcmp(argv[1], "configure") == 0){ //configure
    			int fd = open("configure", O_WRONLY | O_TRUNC | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
    			if(fd == -1){
    				printf("Fatal Error: Could not create the configure file\n");
    				return 0;
    			}
    			writeToFile(fd, argv[2]);
    			writeToFile(fd, " ");
    			writeToFile(fd, argv[3]);
    			writeToFile(fd, "\n");
    			close(fd);
    			printf("Sucessfully created Configure file in current directory\n");
    			
    		}else if(strlen(argv[1]) == 3 && strcmp(argv[1], "add") == 0){ //Add
    		
    		}else if(strlen(argv[1]) == 6 && strcmp(argv[1], "remove") == 0){ //Remove
    		
    		}else if(strlen(argv[1]) == 8 && strcmp(argv[1], "rollback") == 0){ //Rollback
    		
    		}else{
    			printf("Invalid operation\n");
    		}
    	}
    }
    return 0;
}

void metadataParser(int clientfd){
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
	listOfFiles = NULL; 
	char* mode = NULL;
	do{
		read = bufferFill(clientfd, buffer, sizeof(buffer));
		if(buffer[0] == '$'){
			if(mode == NULL){
				mode = token;
				printf("%s\n", mode);
			}else if(strcmp(mode, "ERROR") == 0){
				break;	
			}else if(strcmp(mode, "sendFile") == 0){
				if(numOfFiles == 0){
					numOfFiles = atoi(token);
					listOfFiles = calloc(numOfFiles, sizeof(fileNode));
					free(token);
				}else if(fileName == 0){
					char* temp = NULL;
					fileLength = atoi(token);
					readNbytes(clientfd, fileLength, NULL, &temp);
					listOfFiles[filesRead].filepath = temp;
					char* temp1 = (char*)basename(temp);
					printf("The basename is %s and %lu\n", temp1, strlen(temp1));
					
					char* name = (char*) malloc(sizeof(char) * strlen(temp1) + 1);
					memset(name, '\0', sizeof(char) * strlen(temp1) + 1);
					memcpy(name, temp1, strlen(temp1));
					listOfFiles[filesRead].filename = name;
					free(token);
					fileName = 1;
				}else{
					fileName = 0;
					listOfFiles[filesRead].filelength = (long long) atoi(token);
					filesRead++;
					free(token);
				}
				if(numOfFiles == filesRead){
					int filenumber = 0;
					for(filenumber = 0; filenumber < numOfFiles; ++filenumber){
						printf("File: %s\n", listOfFiles[filenumber].filename);
					}
					break;
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
	}while(buffer[0] != '\0' && read != 0);
}

void readNbytes(int fd, int length, char* mode, char** placeholder){
	char buffer[100] = {'\0'};
	int defaultSize = 15;
	char* token = malloc(sizeof(char) * (defaultSize + 1));
	memset(token, '\0', sizeof(char) * (defaultSize + 1));
	int tokenpos = 0;
	int read = 0;
	int bufferPos = 0;

	do{
		if(length == 0){
			break;
		}else{
			if(length > sizeof(buffer)){
				read = bufferFill(fd, buffer, sizeof(buffer));
			}else{
				read = bufferFill(fd, buffer, length);
			}
		}
		if(mode == NULL){
			for(bufferPos = 0; bufferPos < read; ++bufferPos){
			if(tokenpos >= defaultSize){
				defaultSize = defaultSize * 2;
				token = doubleStringSize(token, defaultSize);
			}
			token[tokenpos] = buffer[bufferPos];
			tokenpos++;
			}
		}
		length = length - read;
	}while(buffer[0] != '\0' && read != 0);
	if(mode == NULL){
		*placeholder = token;
	}
}

void writeToFileFromSocket(int socketfd, fileNode* files){
	char buffer[100] = {'\0'};
	fileNode* file = files;
	while(file != NULL){
		int filelength = file->filelength;
		int read = 0;
		int filefd = open(file->filepath, O_WRONLY);
		if(filefd == -1){
			printf("Fatal Error: Could not write to File from the Socket because File did not exist\n");
		}
		while(filelength != 0){
			if(filelength > sizeof(buffer)){
				read = bufferFill(socketfd, buffer, sizeof(buffer));
			}else{
				read = bufferFill(socketfd, buffer, filelength);
			}
			printf("The buffer has %s\n", buffer);
			writeToFile(filefd, buffer);
			
			filelength = filelength - read;
		}
		file = file->next;
	}
}

void writeToSocketFromFile(int clientfd, char* fileName){
	int filefd = open(fileName, O_RDONLY);
	int read = 0;
	if(filefd == -1){
		printf("Fatal Error: Could not send file because file did not exist\n");
		return;
	}
	char buffer[101] = {'\0'}; // Buffer is sized to 101 because we need a null terminated char array for writeToFile method since it performs a strlen
	do{
		read = bufferFill(filefd, buffer, (sizeof(buffer) - 1)); 
		printf("The buffer has %s\n", buffer);
		writeToFile(clientfd, buffer);
	}while(buffer[0] != '\0' && read != 0);
	printf("Finished sending file: %s to client\n", fileName);
	close(filefd);
}

void createProject(char* directoryName, int socketfd){
	printf("Attempting to create the directory\n");
	int success = makeDirectory(directoryName);
	if(success){
		char* manifest = malloc(sizeof(char) * strlen(directoryName) + 3 + strlen("/Manifest"));
		memset(manifest, '\0', sizeof(char) * strlen(directoryName) + 3 + strlen("/Manifest"));
		manifest[0] = '.';
		manifest[1] = '/';
		memcpy(manifest + 2, directoryName, strlen(directoryName));
		strcat(manifest, "/Manifest");
		int fd = open(manifest, O_WRONLY | O_TRUNC | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
		close(fd);
		free(manifest);
		writeToFileFromSocket(socketfd, listOfFiles);
	}else{
		printf("Client: Directory Failed to Create\n");
	}
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

int setupConnection(){
	char** information = getConfig();
	if(information != NULL){
   	int socketfd = createSocket();
   	if(socketfd != -1){
	  		connectToServer(socketfd, information);
	  	}
		free(information[0]);
		free(information[1]);
		free(information);
	  	return socketfd;
   }
   return 0;
}

int createSocket(){
	int socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if(socketfd < 0){
		printf("Fatal Error: Could not open socket\n");
		return -1;
	}
	return socketfd;
}

int connectToServer(int socketfd, char** serverinfo){
	struct hostent* ip = gethostbyname(serverinfo[0]);
	if(ip == NULL){
		printf("Fatal Error: IP does not exist\n");
		return -1;
	}
	struct sockaddr_in serverinfostruct;
	bzero((char*)&serverinfostruct, sizeof(serverinfostruct)); 
	serverinfostruct.sin_family = AF_INET;
	serverinfostruct.sin_port = htons(atoi(serverinfo[1]));
	bcopy((char*)ip->h_addr, (char*)&serverinfostruct.sin_addr.s_addr, ip->h_length);
	while(1){
		if(connect(socketfd, (struct sockaddr*)&serverinfostruct, sizeof(serverinfostruct)) < 0){
			printf("Error: Cannot connect to server, retrying\n");
			sleep(3);
		}else{
			printf("Successful Connection to Server\n");
			return 1;
		}
	}
	return -1;
}

int generateHash(char* seed){
	int loop = 0;
	int hash = 0;
	for(loop = 0; loop < strlen(seed); ++loop){
		hash = ((hash << 5) - hash) + seed[loop];
	}
	return hash;
}
/*
1st Element = IP
2nd Element = Port
*/
char** getConfig(){
	int fd = open("configure", O_RDONLY);
	if(fd == -1){
		printf("Fatal Error: Configure file is missing, please call configure before running\n");
		return NULL;
	}
	int read = 0;
	char buffer[25];
	int bufferPos = 0;
	int tokenpos = 0;
	int defaultSize = 10;
	char* token = (char*) malloc(sizeof(char) * (defaultSize + 1));
	memset(token, '\0', sizeof(char) * (defaultSize + 1));
	char** information = (char**) malloc(sizeof(char*) * 2);
	memset(information, '\0', sizeof(char*) * 2);
	int finished = 0;
	do{
		read = bufferFill(fd, buffer, sizeof(buffer));
		for(bufferPos = 0; bufferPos < (sizeof(buffer) / sizeof(buffer[0])); ++bufferPos){
			if(buffer[bufferPos] == ' '){
				information[0] = token;
				defaultSize = 10;
				tokenpos = 0;
				token = (char*) malloc(sizeof(char) * (defaultSize + 1));
				memset(token, '\0', sizeof(char) * (defaultSize + 1));
			}else if(buffer[bufferPos] == '\n'){
				information[1] = token;
				finished = 1;
				break;
			}else{
				if(tokenpos >= defaultSize){
					defaultSize = defaultSize * 2;
					token = doubleStringSize(token, defaultSize);
				}
				token[tokenpos] = buffer[bufferPos];
				tokenpos++;
			}
		}
	}while((buffer[0] != '\0' && finished == 0) && read != 0);
	return information;
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
       		break;
        }else if(status == -1){
            printf("Warning: Error when reading the file reading\n");
            return bytesRead;
        }
        bytesRead += status;
    }while(bytesRead < bytesToRead);
    	return bytesRead;
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

