#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/socket.h>
#include <netdb.h>
#include <openssl/md5.h>
#include <libgen.h>
#include <stdlib.h>
#include <stdio.h>


typedef struct _fileNode_{
	char* filename;
	char* filepath;
	long long filelength;
	struct _fileNode_* next;
	struct _fileNode_* prev;
}fileNode;

typedef struct _manifestNodes_{
	char* filepath;
	char* version;
	char* hash;
	struct _manifestNodes_* next;
	struct _manifestNodes_* prev;
}mNode;

/*
	Client Setup Methods
*/
int connectToServer(int socketfd, char** serverinfo);
int createSocket();
int setupConnection();
char** getConfig();

/*
	Cleanup Methods
*/
void freeFileNodes();

/*
	Helper Methods
*/
int directoryExist(char* directoryPath);
void createDirectories(fileNode* list);
int makeDirectory(char* directoryName);
void makeNestedDirectories(char* path);
char* doubleStringSize(char* word, int newsize);
void insertLL(fileNode* node);
void appendToManifest(char* ProjectName, char* token);
char* createManifestLine(char* version, char* filepath, char* hashcode, int local, int mode);
char* generateManifestPath(char* projectName);
void removeFromManifest(char* ProjectName, char* filepath);
char* generateHashCode(char* filepath);
mNode* insertMLL(mNode* newNode, mNode* head);
void printMLL(mNode* head);
void quickSortRecursive(mNode* startNode, mNode* endNode, int (*comparator)(void*, void*));
int quickSort( mNode* head, int (*comparator)(void*, void*));
void* partition(mNode* startNode, mNode* endNode, int (*comparator)(void*, void*));
int strcomp(void* string1, void* string2); //Use as comparator
/*
	File Sending Methods
*/
void writeToSocketFromFile(int clientfd, char* fileName);
void sendLength(int socketfd, char* token);
void writeToFile(int fd, char* data);

/*
	File Recieving Methods
*/
void metadataParser(int clientfd);
void readNbytes(int fd, int length, char* mode, char** placeholder);
void writeToFileFromSocket(int socketfd, fileNode* files);
int bufferFill(int fd, char* buffer, int bytesToRead);
/*
	Commands Methods
*/
void createProject(char* directoryName, int socketfd);
void update(char* ProjectName, int socketfd);

fileNode* listOfFiles = NULL;
int numOfFiles = 0;

int main(int argc, char** argv) {
    if(argc != 4 && argc != 3){
    	printf("Fatal Error: Incorrect number of arguments.\n");    
    }else{
    	if(argc == 3){
    		if(strlen(argv[1]) == 8 && strcmp(argv[1], "checkout") == 0){  //checkout
    			if(directoryExist(argv[2]) == 1){
    				printf("Fatal Error: Project already exist on client side\n");
    				return 0;
    			}
    			int socketfd  = setupConnection();
    			if(socketfd > 0){
    				writeToFile(socketfd, "checkout$");
					sendLength(socketfd, argv[2]);
					char* temp = NULL;
					readNbytes(socketfd, strlen("FAILURE"), NULL, &temp);
					if(strcmp(temp, "SUCCESS") == 0){
						printf("Server successfully located the project, recieving the project\n");
						metadataParser(socketfd);
						createDirectories(listOfFiles);
						writeToFileFromSocket(socketfd, listOfFiles);
					}else if(strcmp(temp, "FAILURE") == 0){
						printf("Error: Server failed to locate the project, either project did not exist or had no permissions to access the project\n");
					}else{
						printf("Error: Could not interpret the server's response\n");
					}
					free(temp);
					close(socketfd);
    			}else{
    			
    			}
    		}else if(strlen(argv[1]) == 6 && strcmp(argv[1], "update") == 0){ //UPDATE
    			int socketfd = setupConnection();
    			if(socketfd > 0){
    				writeToFile(socketfd, "update$");
    				sendLength(socketfd, argv[2]);
    				char* temp = NULL;
    				readNbytes(socketfd, strlen("FAILURE"), NULL, &temp);
    				if(strcmp(temp, "SUCCESS") == 0){
    					printf("Server sucessfully located the project's manifest, recieving the server manifest\n");
    					metadataParser(socketfd);
    					update(argv[2], socketfd);
    				}else if(strcmp(temp , "FAILURE") == 0){
    					printf("Error: Server failed to locate the project's manifest\n");
    				}else{
    					printf("Error: Could not interpret the server's response\n");
    				}
    				close(socketfd);
    			}else{
    			
    			}
    		}else if(strlen(argv[1]) == 7 && strcmp(argv[1], "upgrade") == 0){ //upgrade
    			
    		}else if(strlen(argv[1]) == 6 && strcmp(argv[1], "commit") == 0){ //commit
    			
    		}else if(strlen(argv[1]) == 4 && strcmp(argv[1], "push") == 0){ //push
    			
    		}else if(strlen(argv[1]) == 6 && strcmp(argv[1], "create") == 0){ //create
				int socketfd = setupConnection();
				if(socketfd > 0){
					writeToFile(socketfd, "create$");
					sendLength(socketfd, argv[2]);
					char* temp = NULL;
					readNbytes(socketfd, strlen("FAILURE"), NULL, &temp);
					if(strcmp(temp, "SUCCESS") == 0){
						printf("Server sucessfully created the project, recieving the Manifest\n");
						metadataParser(socketfd);
						createProject(argv[2], socketfd);
					}else if(strcmp(temp , "FAILURE") == 0){
						printf("Error: Server failed to created the project\n");
					}else{
						printf("Error: Could not interpret the server's response\n");
					}
					free(temp);
					close(socketfd);
				}else{
				
				}
    		}else if(strlen(argv[1]) == 7 && strcmp(argv[1], "destroy") == 0){ //destroy
    			int socketfd = setupConnection();
				if(socketfd > 0){
					writeToFile(socketfd, "destroy$");
					sendLength(socketfd, argv[2]);
					char* temp = NULL;
					readNbytes(socketfd, strlen("FAILURE"), NULL, &temp);
					if(strcmp(temp, "SUCCESS") == 0){
						printf("Server sucessfully destroyed the project\n");
					}else if(strcmp(temp , "FAILURE") == 0){
						printf("Fatal Error: Server failed to destroy the project\n");
					}else{
						printf("Fatal Error: Could not interpret the server's response\n");
					}
					free(temp);
					close(socketfd);
				}else{
				
				}
    		}else if(strlen(argv[1]) == 14 && strcmp(argv[1], "currentversion") == 0){ //currentversion
				int socketfd = setupConnection();
				if(socketfd > 0){
					writeToFile(socketfd, "currentversion$");
					sendLength(socketfd, argv[2]);
 					char* temp = NULL;
					readNbytes(socketfd, strlen("FAILURE"), NULL, &temp);
					if(strcmp(temp, "SUCCESS") == 0){
						char* response = NULL;
						printf("Server Succesfully located the project, recieving the files and versions\n");
						metadataParser(socketfd);
					}else if(strcmp(temp, "FAILURE") == 0){
						printf("Error: Server could not locate the project\n");
					}else{
						printf("Error: Could not interpret the server's response\n");
					}
					free(temp);
               close(socketfd);
				}else{
						
				}
				
    		}else if(strlen(argv[1]) == 7 && strcmp(argv[1], "history") == 0){ //history
    			
    		}else{
    			printf("Fatal Error: Invalid operation or Improperly Formatted\n");
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
    			if(directoryExist(argv[2]) == 0){
    				printf("Fatal Error: Project does not exist to add the file\n");
    			}else{
    				char* hashcode = generateHashCode(argv[3]);
    				if(hashcode == NULL){
    					
    				}else{
		 				char* temp = createManifestLine("1", argv[3], hashcode, 1, 1); 
		 				appendToManifest(argv[2], temp);
		 				free(hashcode);
		 				free(temp);
    				}
    				
    			}
    			
    		}else if(strlen(argv[1]) == 6 && strcmp(argv[1], "remove") == 0){ //Remove
    			if(directoryExist(argv[2]) == 0){
    				printf("Fatal Error: Project does not exist to remove the file\n");
    			}else{
    				removeFromManifest(argv[2], argv[3]);
    			}
    		}else if(strlen(argv[1]) == 8 && strcmp(argv[1], "rollback") == 0){ //Rollback
    		
    		}else{
    			printf("Fatal Error: Invalid operation or Improperly Formatted\n");
    		}
    	}
    }
    freeFileNodes();
    return 0;
}

void sendLength(int socketfd, char* token){
	char str[5] = {'\0'};
	sprintf(str, "%lu", strlen(token));
	writeToFile(socketfd, str);
	writeToFile(socketfd, "$");
	writeToFile(socketfd, token);
}
/*
Purpose: Parses the metadata
	$ is the delimiter
	Modes: 
		- output
		Format: output$dataBytes$data
		- sendFile
		Format: sendFile$numOfFiles$file1_name_bytes$file1name sizeoffile1$(...repeat for number of files...) file1contentfile2contentfile3content (...repeat for number of files...)
		*Note the space between filename and sizeoffile is added for readablility, it should not have a space and () indicates the previous pattern repeats for the number of files, everything should be one continous token
*/
void metadataParser(int clientfd){
	char buffer[2] = {'\0'}; 
	int defaultSize = 15;
	char* token = malloc(sizeof(char) * (defaultSize + 1));
	memset(token, '\0', sizeof(char) * (defaultSize + 1));
	int read = 0;
	int bufferPos = 0;
	int tokenpos = 0;
	int filesRead = 0;
	int readName = 0; 
	listOfFiles = NULL; 
	fileNode* file = NULL;
	numOfFiles = 0;
	char* mode = NULL;
	do{
		read = bufferFill(clientfd, buffer, 1);
		if(buffer[0] == '$'){
			if(mode == NULL){
				mode = token;
				printf("%s\n", mode);
			}else if(strcmp(mode, "output") == 0){
				int responseLength = atoi(token);
				free(token);
				readNbytes(clientfd, responseLength, NULL, NULL);
				break;
			}else if(strcmp(mode, "sendFile") == 0){
				if(numOfFiles == 0){
					numOfFiles = atoi(token);
					
					file = (fileNode*) malloc(sizeof(fileNode) * 1);
					file->next = NULL;
					file->prev = NULL;
					
					free(token);
				}else if(readName == 0){
					printf("The length of the file path is: %s\n", token);
					char* temp = NULL;
					int filepathlength = atoi(token);
					readNbytes(clientfd, filepathlength, NULL, &temp);
					file->filepath = temp;
					
					char* name = (char*) malloc(sizeof(char) * strlen(basename(temp)) + 1);
					memset(name, '\0', sizeof(char) * strlen(basename(temp)) + 1);
					memcpy(name, basename(temp), strlen(basename(temp)));
					file->filename = name;
					
					free(token);
					readName = 1;
				}else{
					file->filelength = (long long) atoi(token);
					insertLL(file);
					printf("The file is %s with path of %s\n", file->filename, file->filepath);
					file = (fileNode*) malloc(sizeof(fileNode) * 1);
					file->next = NULL;
					file->prev = NULL;
					
					filesRead++;
					free(token);
					readName = 0;
				}
				if(numOfFiles == filesRead){
					free(file);
					fileNode* temp = listOfFiles;
					while(temp != NULL){
						printf("File: %s\n", temp->filename);
						temp = temp->next;
					}
					break;
				}
			}else{
			
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
		if(mode != NULL){
			free(mode);
		}
}

/*
	Automatically creates the Manifest file and calls writeToFileFromSocket to read the socket and store the data in the files
	Assumes metadataParser was called before to create the listOfFiles
*/
void createProject(char* directoryName, int socketfd){
	printf("Attempting to create the directory\n");
	int success = makeDirectory(directoryName);
	if(success){
		char* manifest = generateManifestPath(directoryName);
		int fd = open(manifest, O_WRONLY | O_TRUNC | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
		close(fd);
		free(manifest);
		writeToFileFromSocket(socketfd, listOfFiles);
	}else{
		printf("Client: Directory Failed to Create\n");
	}
}
/*
	Assuming MetadataParser is filled
*/
void update(char* ProjectName, int socketfd){
	char* manifest = generateManifestPath(ProjectName);
	int manifestfd = open(manifest, O_RDONLY);
	if(manifestfd == -1){
		printf("Fatal Error: Manifest does not exist or does not have permissions to be opened\n");
		return;
	}
	char buffer[100] = {'\0'};
   int defaultSize = 15;

    
   int read = 0;
   int tokenpos = 0;
   int bufferPos = 0;
   int numOfSpaces = 0;
    
   int manifestVersion = 0;
   
   char* serverVersion = malloc(sizeof(char) * (defaultSize + 1));
   memset(serverVersion, '\0', sizeof(char) * (defaultSize + 1));
   int length = listOfFiles->filelength;
   int sameVersion = 0;
   
   mNode* serverHead = NULL;
   mNode* clientHead = NULL;
   mNode* curLine = malloc(sizeof(mNode) * 1);
   
   do{
		read = bufferFill(socketfd, buffer, 1);
		length = length - read;
		if(buffer[0] == '\n'){
			break;
		}
   	if(tokenpos >= defaultSize){
			defaultSize = defaultSize * 2;
			serverVersion = doubleStringSize(serverVersion, defaultSize);
		}
		serverVersion[tokenpos] = buffer[0];
		tokenpos++;
   }while(read != 0);
   defaultSize = 15;
	tokenpos = 0;
	char *token = malloc(sizeof(char) * (defaultSize + 1));
   memset(token, '\0', sizeof(char) * (defaultSize + 1));
	
	do{
		read = bufferFill(manifestfd, buffer, sizeof(buffer));
		for(bufferPos = 0; bufferPos < (sizeof(buffer)/sizeof(buffer[0])); ++bufferPos){
			if(buffer[bufferPos] == '\n'){ 
				if(manifestVersion == 0){ //This is for version of the Manifest 
					manifestVersion = 1; 
					if(strcmp(token, serverVersion) == 0){
						sameVersion = 1;
						break;
					}
					free(serverVersion);
					free(token);
		     	}else{
		     		curLine->hash = token;
		     		clientHead = insertMLL(curLine, clientHead);
		     		curLine = (mNode*) malloc(sizeof(mNode) * 1);
		     	}
		     	defaultSize = 15;
    			token = malloc(sizeof(char) * (defaultSize + 1));
   			memset(token, '\0', sizeof(char) * (defaultSize + 1));
   			tokenpos = 0;
		     	numOfSpaces = 0;
			}else if(buffer[bufferPos] == ' '){
		    	numOfSpaces++;
		    	if(numOfSpaces == 1){
		    		curLine->version = token;
		    	}else if(numOfSpaces == 2){
		    		curLine->filepath = token;
		    	}
		    	token = malloc(sizeof(char) * (defaultSize + 1));
   			memset(token, '\0', sizeof(char) * (defaultSize + 1));
   			tokenpos = 0;
			}else{
		    	if(tokenpos >= defaultSize){
					defaultSize = defaultSize * 2;
					token = doubleStringSize(token, defaultSize);
				}
				token[tokenpos] = buffer[bufferPos];
				tokenpos++;
		   }
		}
	}while (buffer[0] != '\0' && read != 0 && sameVersion != 1);
	defaultSize = 15;
	tokenpos = 0;
	if(sameVersion == 1){
	
	}else{
	do{
		if(length == 0){
			break;
		}else{
			if(length > sizeof(buffer)){
				read = bufferFill(socketfd, buffer, sizeof(buffer));
			}else{
				memset(buffer, '\0', sizeof(buffer));
				read = bufferFill(socketfd, buffer, length);
			}
		}
		for(bufferPos = 0; bufferPos < (sizeof(buffer)/sizeof(buffer[0])); ++bufferPos){
			if(buffer[bufferPos] == '\n'){ 
		     	curLine->hash = token;
		     	serverHead = insertMLL(curLine, serverHead);
		     	curLine = (mNode*) malloc(sizeof(mNode) * 1);
		     	defaultSize = 15;
    			token = malloc(sizeof(char) * (defaultSize + 1));
   			memset(token, '\0', sizeof(char) * (defaultSize + 1));
   			tokenpos = 0;
		     	numOfSpaces = 0;
			}else if(buffer[bufferPos] == ' '){
		    	numOfSpaces++;
		    	if(numOfSpaces == 1){
		    		curLine->version = token;
		    	}else if(numOfSpaces == 2){
		    		curLine->filepath = token;
		    	}
		    	defaultSize = 15;
		    	token = malloc(sizeof(char) * (defaultSize + 1));
   			memset(token, '\0', sizeof(char) * (defaultSize + 1));
   			tokenpos = 0;
			}else{
		    	if(tokenpos >= defaultSize){
					defaultSize = defaultSize * 2;
					token = doubleStringSize(token, defaultSize);
				}
				token[tokenpos] = buffer[bufferPos];
				tokenpos++;
		   }
		}
		length = length - read;
	}while (buffer[0] != '\0' && read != 0 && length != 0);
	}
	quickSort(serverHead, strcomp);
	quickSort(clientHead, strcomp);
	printf("ServerHead:\n");
	printMLL(serverHead);
	printf("clientHead:\n");
	printMLL(clientHead);
	free(curLine); //CHANGE TO mNode FREE METHOD
}

void printMLL(mNode* head){
	mNode* temp = head;
	while(temp != NULL){
		printf("Version:%s\tFilepath:%s\tHashcode:%s\n", temp->version, temp->filepath, temp->hash);
		temp = temp->next;
	}
}

mNode* insertMLL(mNode* newNode, mNode* head){
	newNode->next = NULL;
	newNode->prev = NULL;
	if(head == NULL){
	
	}else{
		newNode->next = head;
		head->prev = newNode;
	}
	return newNode;
}

/*
	Reads N bytes specified by length
		IF MODE = NULL: creates a token and stores into placeholder
		IF MODE = NULL AND placeholder = NULL, outputs to the terminal
*/
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
		if(mode == NULL && placeholder != NULL){
			for(bufferPos = 0; bufferPos < read; ++bufferPos){
			if(tokenpos >= defaultSize){
				defaultSize = defaultSize * 2;
				token = doubleStringSize(token, defaultSize);
			}
			token[tokenpos] = buffer[bufferPos];
			tokenpos++;
			}
		}else if(mode == NULL && placeholder == NULL){
			printf("%s", buffer);
		}
		length = length - read;
	}while(buffer[0] != '\0' && read != 0);
	if(mode == NULL && placeholder != NULL){
		*placeholder = token;
	}else if(mode == NULL && placeholder == NULL){
		free(token);
	}
}

/*
Purpose: Reads data from the socket and writes to the files
	Assuming files is an linked list of fileNode* (generated by metadataParser)
*/
void writeToFileFromSocket(int socketfd, fileNode* files){
	fileNode* file = files;
	while(file != NULL){
		char buffer[100] = {'\0'};
		int filelength = file->filelength;
		int read = 0;
		int filefd = open(file->filepath, O_WRONLY | O_CREAT,  S_IRUSR | S_IWUSR);
		if(filefd == -1){
			printf("Fatal Error: Could not write to File from the Socket because File did not exist or no permissions\n");
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

/*
Purpose: Reads data from a single file and writes to the socket
*/
void writeToSocketFromFile(int clientfd, char* fileName){
	int filefd = open(fileName, O_RDONLY);
	int read = 0;
	if(filefd == -1){
		printf("Fatal Error: Could not send file because file did not exist or no permissions\n");
		return;
	}
	char buffer[101] = {'\0'}; // Buffer is sized to 101 because we need a null terminated char array for writeToFile method since it performs a strlen
	do{
		read = bufferFill(filefd, buffer, (sizeof(buffer) - 1)); 
		printf("The buffer has %s\n", buffer);
		writeToFile(clientfd, buffer);
	}while(buffer[0] != '\0' && read != 0);
	printf("Finished sending file: %s to Server\n", fileName);
	close(filefd);
}

/*
	Given a buffer and bytesToRead, it will attempt to read the fd and fill the buffer
		bytesToRead should be <= sizeof(buffer)
*/
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

/*
	Given a null terminated char array, it will write all bytes from this array to the file
*/
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

void removeFromManifest(char* projectName, char* filepath){
	char* manifest = generateManifestPath(projectName);
	char* manifestTemp = malloc(sizeof(char) * (strlen(manifest) + 5));
	memset(manifestTemp, '\0', sizeof(char) * (strlen(manifest) + 5));
	memcpy(manifestTemp, manifest, strlen(manifest));
	strcat(manifestTemp, "temp");
	
	int manifestfd = open(manifest, O_RDONLY); 
	
	if(manifestfd == -1){
		printf("Fatal Error: The Manifest does not exist!\n");
		return;
	}
	int tempmanifestfd = open(manifestTemp, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	if(tempmanifestfd == -1){
		printf("File already exists somehow\n");
	}
	int defaultSize = 25;
	char* token = malloc(sizeof(char) * (defaultSize + 1));
	memset(token, '\0', sizeof(char) * (defaultSize + 1));
	
	char buffer[125] = {'\0'};
	int tokenpos = 0;
	int bufferPos = 0;
	int read = 0;
	int found = -1;
	int existed = -1;
	do{
		read = bufferFill(manifestfd, buffer, sizeof(buffer));
		for(bufferPos = 0; bufferPos < sizeof(buffer); ++bufferPos){
			if(tokenpos >= defaultSize){
				defaultSize = defaultSize * 2;
				token = doubleStringSize(token, defaultSize);
			}
			token[tokenpos] = buffer[bufferPos];
			tokenpos++;
			if(buffer[bufferPos] == '\n'){
				char* temp = malloc(sizeof(char) * strlen(token));
				memset(temp, '\0', strlen(token) * sizeof(char));
				int i = 0;
				int numofspaces = 0;
				int temppos = 0;
				for(i = 0; i < tokenpos; ++i){
					if(token[i] == ' '){
						numofspaces++;
						if(numofspaces == 2){
							found = strcmp(filepath, temp);
							if(found == 0){
								existed = 1;
							}
							break;
						}
					}else if(numofspaces == 1){
						temp[temppos] = token[i];
						temppos++;
					}
				}
				printf("%s\n", temp);
				free(temp);
				if(found != 0){
					writeToFile(tempmanifestfd, token);
				}
				found = -1;
				free(token);
				defaultSize = 25;
				token = malloc(sizeof(char) * (defaultSize + 1));
				memset(token, '\0', sizeof(char) * (defaultSize + 1));
				tokenpos = 0;
			}
		}
	}while(buffer[0] != '\0' && read != 0);
	
	close(manifestfd);
	close(tempmanifestfd);
	if(existed == -1){
		printf("Warning: The file is not listed in the Manifest\n");
		int success = remove(manifestTemp);
		printf("%d\n", success);
	}else{
		remove(manifest);
		rename(manifestTemp, manifest);
	}
	free(token);
	free(manifest);
	free(manifestTemp);	
}

char* generateManifestPath(char* projectName){
	char* manifest = malloc(sizeof(char) * strlen(projectName) + 3 + strlen("/Manifest"));
	memset(manifest, '\0', sizeof(char) * strlen(projectName) + 3 + strlen("/Manifest"));
	manifest[0] = '.';
	manifest[1] = '/';
	memcpy(manifest + 2, projectName, strlen(projectName));
	strcat(manifest, "/Manifest");
	return manifest;
}

void appendToManifest(char* projectName, char* token){
	char* manifest = generateManifestPath(projectName);
	int fd = open(manifest, O_WRONLY | O_APPEND);
	if(fd == -1){
		printf("Fatal Error: Manifest was not found the project file\n");
	}else{
		writeToFile(fd, token);
		close(fd);
	}
	free(manifest);
}

/*
Local 0: Indicates the file is not just local added
Local 1: Indicates the file is locally added
Mode 0: will just create a line of the Manifest given the version/filepath/hashcode
Mode 1: For 'add', will append l before versionNumber to indicate it was locally added
Mode 2: For 'remove', will append rl before versionNumber to indicate it was locally removed
*/
char* createManifestLine(char* version, char* filepath, char* hashcode, int local, int mode){
	char* line = NULL;
	if(local){
		line = (char*) malloc(sizeof(char) * (strlen(version) + strlen(filepath) + strlen(hashcode) + 6)); // 1 for null terminal, 3 for spaces/ 2 to indicate if it was created locally
		memset(line, '\0', sizeof(char) * (strlen(version) + strlen(filepath) + strlen(hashcode) + 6));
	}else{
		line = (char*) malloc(sizeof(char) * (strlen(version) + strlen(filepath) + strlen(hashcode) + 4));
		memset(line, '\0', sizeof(char) * (strlen(version) + strlen(filepath) + strlen(hashcode) + 4));
	}
	if(mode == 1){
		char* temp = (char*) malloc(sizeof(char) * (strlen(version) + 2));
		memset(temp, '\0', sizeof(char) * (strlen(version) + 2));
		temp[0] = 'l';
		strcat(temp, version);
		memcpy(line, temp, strlen(temp));
		line[strlen(temp)] = ' ';
		memcpy(line+strlen(temp) + 1, filepath, strlen(filepath));
		line[strlen(temp) + 1 + strlen(filepath)] = ' ';
		memcpy(line+strlen(temp) + 1 + strlen(filepath) + 1, hashcode, strlen(hashcode));
		free(temp);
	}else if(mode == 2){
		char* temp = (char*) malloc(sizeof(char) * (strlen(version) + 3));
		memset(temp, '\0', sizeof(char) * (strlen(version) + 3));
		temp[0] = 'l';
		temp[1] = 'r';
		strcat(temp, version);
		memcpy(line, temp, strlen(temp));
		line[strlen(temp)] = ' ';
		memcpy(line+strlen(temp) + 1, filepath, strlen(filepath));
		line[strlen(temp) + 1 + strlen(filepath)] = ' ';
		memcpy(line+strlen(temp) + 1 + strlen(filepath) + 1, hashcode, strlen(hashcode));
		free(temp);
	}else{
		memcpy(line, version, strlen(version));
		line[strlen(version)] = ' ';
		memcpy(line+strlen(version) + 1, filepath, strlen(filepath));
		line[strlen(version) + 1 + strlen(filepath)] = ' ';
		memcpy(line+strlen(version) + 1 + strlen(filepath) + 1, hashcode, strlen(hashcode));
	}
	line[strlen(line)] = '\n';
	printf("Inserting:%s", line);
	return line;
}

char* generateHashCode(char* filepath){
	int filefd = open(filepath, O_RDONLY);
	if(filefd == -1){
		printf("Fatal Error: File does not exist to generate a hashcode\n");
		return NULL;
	}else{
		char* hash = (char *) malloc(sizeof(char) * (MD5_DIGEST_LENGTH + 1));	
		memset(hash, '\0', sizeof(char) * (MD5_DIGEST_LENGTH + 1));
		char buffer[1024] = {'\0'};
		int read = 0;
		MD5_CTX mdHash;
		MD5_Init (&mdHash);
		do{
			read = bufferFill(filefd, buffer, sizeof(buffer));
			MD5_Update(&mdHash, buffer, read);
		}while(buffer[0] != '\0' && read != 0);
		MD5_Final (hash, &mdHash);
		int i = 0;
		printf("The hash for this file is: ");
		for(i = 0; i < MD5_DIGEST_LENGTH; i++){
			printf("%02x", (unsigned char) hash[i]);
		}
		printf("\n");
		char* hexhash = (char *) malloc(sizeof(char) * ((strlen(hash) * 2) + 1));
		memset(hexhash, '\0', sizeof(char) * ((strlen(hash) * 2) + 1));
		int previous = 0;
		for(i = 0; i < strlen(hash); ++i){
			sprintf( (char*) (hexhash + previous), "%02x", (unsigned char) hash[i]); //Each characters takes up 2 bytes now (hexadecimal value)
			previous += 2;
		}
		free(hash);
		return hexhash;
	}
}


void createDirectories(fileNode* list){
	fileNode* file = list;
	while(file != NULL){
		char* temp = strdup(file->filepath);
		char* subdirectories = dirname(temp);
		makeNestedDirectories(subdirectories);
		free(temp);
		file = file->next;
	}
}

/*
	Creates a single directory, given the directory path (does not work with nested directories)
*/
int makeDirectory(char* directoryPath){
	int success = mkdir(directoryPath, S_IRWXU);
	if(success == -1){
		printf("Warning: Directory could not be created\n");
		return 0;
	}else{
		printf("Directory Created\n");
		return 1;
	}
}

int directoryExist(char* directoryPath){
	DIR* directory = opendir(directoryPath);
	if(directory != NULL){ //Directory Exists
		closedir(directory);
		return 1;
	}else{ //Directory does not exist or does not have permission to access
		return 0;
	}
}

/*
	Given a directory path, will construct the whole directory path (including the nested directories) and will not overwrite directories that exist
*/
void makeNestedDirectories(char* path){ 
	char* parentdirectory = strdup(path);
	char* directory = strdup(path);
	char* directoryName = basename(directory);
	char* parentdirectoryName = dirname(parentdirectory);
	if(strlen(path) != 0 && strcmp(path, ".") != 0){
		makeNestedDirectories(parentdirectoryName);
	}
	mkdir(path, 0777);
	free(directory);
	free(parentdirectory);
}

/*
	Inserts the fileNode at the tail (to perverse the order)
*/
void insertLL(fileNode* node){
	if(listOfFiles == NULL){
		listOfFiles = node;
	}else{
		fileNode* temp = listOfFiles;
		while(temp->next != NULL){
			temp = temp->next;
		}
		temp->next = node;
		node->prev = temp;
	}
}

int setupConnection(){
	char** information = getConfig();
	if(information != NULL){
   	int socketfd = createSocket();
   	if(socketfd >= 0){
	  		if(connectToServer(socketfd, information) == -1){
	  			close(socketfd);
	  			socketfd = -1;
	  		}
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
	}
	return socketfd;
}

int connectToServer(int socketfd, char** serverinfo){
	struct hostent* ip = gethostbyname(serverinfo[0]); //Not sure how to handle this, stuff isn't being freed here
	if(ip == NULL){
		printf("Fatal Error: Could not locate IP address\n");
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

/*
Returns a char** that contains:
	1st Element = IP
	2nd Element = Port
*/
char** getConfig(){
	int fd = open("configure", O_RDONLY);
	if(fd == -1){
		printf("Fatal Error: Configure file is missing or no permissions to Configure file, please call configure before running\n");
		return NULL;
	}
	int read = 0;
	char buffer[25] = {'\0'};
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


void freeFileNodes(){
	while(listOfFiles != NULL){
		free(listOfFiles->filename);
		free(listOfFiles->filepath);
		fileNode* temp = listOfFiles;
		listOfFiles = listOfFiles->next;
		free(temp);
	}
	listOfFiles = NULL;
}

char* doubleStringSize(char* word, int newsize){
	char* expanded =  (char*) malloc(sizeof(char) * (newsize + 1));
	memset(expanded, '\0', (newsize+1) * sizeof(char));
	memcpy(expanded, word, strlen(word));
	free(word);
	return expanded;
}

void quickSortRecursive(mNode* startNode, mNode* endNode, int (*comparator)(void*, void*)){
    if(startNode == endNode || startNode == NULL || endNode == NULL){
        return;
    }

    mNode* prevPivot = partition(startNode, endNode, comparator);
    if(prevPivot != startNode){
        quickSortRecursive(startNode, prevPivot->prev, comparator);
    }
    if(prevPivot != endNode){
        quickSortRecursive(prevPivot->next, endNode, comparator);
    }

}

int quickSort( mNode* head, int (*comparator)(void*, void*)){
    mNode* tail = head;
    while(tail->next != NULL){
        tail = tail->next;
    }
    quickSortRecursive(head, tail, comparator);
    return 1;
}

void* partition(mNode* startNode, mNode* endNode, int (*comparator)(void*, void*)){
    mNode* pivot = startNode;

    mNode* left = pivot->next;
    mNode* end = endNode;
    mNode* storeIndex = left;
    mNode* beforeNULL = end;


    mNode* i;
    for(i = left; i != end->next; i = i->next){
        if(comparator(i, pivot) <= 0){
            beforeNULL = storeIndex;
            char* holder = i->filepath;
            i->filepath = storeIndex->filepath;
            storeIndex->filepath = holder;
            storeIndex = storeIndex->next;
        }
    }
    char* holder = pivot->filepath;
    if(storeIndex == NULL){
        pivot->filepath = beforeNULL->filepath;
        beforeNULL->filepath = holder;
        return beforeNULL;
    }else{
        pivot->filepath = storeIndex->prev->filepath;
        storeIndex->prev->filepath = holder;
        return storeIndex->prev;
    }
    return NULL; //ERROR IF IT REACHES HERE
}

int strcomp(void* string1, void* string2){
    mNode* s1ptr = (mNode*) string1;
    mNode* s2ptr = (mNode*) string2;

    char* s1 = s1ptr->filepath;
    char* s2 = s2ptr->filepath;
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