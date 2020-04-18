#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <signal.h>
#include <openssl/md5.h>
#include <libgen.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct _fileNode_{
	char* filename;
	char* filepath;
	long long filelength;
	struct _fileNode_* next;
	struct _fileNode_* prev;
}fileNode;

typedef struct _userNode_{
	pthread_t thread;
	struct _userNode_* next;
}userNode;

/*
	Clean up Methods
*/
void sighandler(int sig);
void unloadMemory();

/*
	Setup Server Methods
*/
int setupServer(int socketfd, int port);
/*
	File Recieving Methods:
*/
void* metadataParser(void* clientfdptr);
int bufferFill(int fd, char* buffer, int bytesToRead);
void readNbytes(int fd, int length,char* mode ,char** placeholder);

/*
	File Sending Methods:
*/
void writeToFile(int fd, char* data);
void sendFile(int clientfd, char* fileName);
void setupFileMetadata(int clientfd, fileNode* files, int numOfFiles);
void sendFilesToClient(int clientfd, fileNode* files, int numOfFiles);

/*
	Helper Methods
*/
char* doubleStringSize(char* word, int newsize);
char* pathCreator(char* path, char* name);
int makeDirectory(char* directoryName);
void makeNestedDirectories(char* path);
int directoryTraverse(char* path, int mode, int fd);
void calculateFileBytes(char* fileName, fileNode* file);
fileNode* createFileNode(char* filepath, char* filename);
void readManifestFiles(char* projectName, int mode, int clientfd);
char* createManifestLine(char* version, char* filepath, char* hashcode, int local, int mode);
void appendToManifest(char* ProjectName, char* token);
char* generateManifestPath(char* projectName);
void printFiles();
void insertLL(fileNode* node);
char* generateHashCode(char* filepath);
void insertThreadLL(pthread_t node);
/*
	Command Methods
*/
void createProject(char* directoryName, int clientfd);
void getProjectVersion(char* directoryName, int clientfd);
void createManifest(int fd, char* directorypath);
void destroyProject(char* directoryName, int clientfd);

userNode* pthreadHead = NULL;

fileNode* listOfFiles = NULL;
int numOfFiles = 0;

int main(int argc, char** argv){
	signal(SIGINT, sighandler);
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
				printf("Error code: %d\n", errno);
			}else{
				struct sockaddr_in client;
				socklen_t clientSize = sizeof(struct sockaddr_in);
				while(1){
					int clientfd = accept(socketfd, (struct sockaddr*) &client, &clientSize);
					if(clientfd == -1){
						printf("Error: Refused Connection\n");
					}else{
						printf("Successfully: Accepted Connection\n");
						printf("Creating the thread to run this connection\n");
						pthread_t client = NULL;
						pthread_create(&client, NULL, metadataParser, &clientfd);
						insertThreadLL(client);
					}
				}
			}
		}
	}
	return 0;
}

void insertThreadLL(pthread_t node){
	userNode* newNode = malloc(sizeof(userNode) * 1);
	newNode->thread = node;
	newNode->next = NULL;
	
	if(pthreadHead == NULL){
		pthreadHead = newNode;
	}else{
		newNode->next = pthreadHead;
		pthreadHead = newNode;
	}
}

void* metadataParser(void* clientfdptr){
	int clientfd = *((int*) clientfdptr);
	char buffer[1] = {'\0'}; 
	int defaultSize = 15;
	char* token = malloc(sizeof(char) * (defaultSize + 1));
	memset(token, '\0', sizeof(char) * (defaultSize + 1));
	int read = 0;
	int bufferPos = 0;
	int tokenpos = 0;
	int fileLength = 0;
	int filesRead = 0;
	int fileName = 0;
	listOfFiles = NULL;
	fileNode* file = NULL;
	char* mode = NULL;
	do{
		read = bufferFill(clientfd, buffer, sizeof(buffer));
		if(buffer[0] == '$'){
			if(mode == NULL){
				mode = token;
				printf("%s\n", mode);
			}else if(strcmp(mode, "destroy") == 0){
				printf("Reading the project to destroy\n");
				fileLength = atoi(token);
				free(token);
				char* temp = NULL;
				readNbytes(clientfd, fileLength, NULL, &temp);
				destroyProject(temp, clientfd);
				free(temp);
			}else if(strcmp(mode, "checkout") == 0){
				printf("Reading the project to check out\n");
				fileLength = atoi(token);
				free(token);
				char* temp = NULL;
				readNbytes(clientfd, fileLength, NULL, &temp);
				readManifestFiles(temp, 0, clientfd);
				printFiles();
				sendFilesToClient(clientfd, listOfFiles, numOfFiles);
				free(temp);
			}else if(strcmp(mode, "create") == 0) {
				printf("Reading the filename to create\n");
				fileLength = atoi(token);
				free(token);
				char* temp = NULL;
				readNbytes(clientfd, fileLength, NULL, &temp);
				createProject(temp, clientfd);
				free(temp);
				break;
         }else if(strcmp(mode, "currentversion") == 0){
             printf("Reading the filename\n");
             fileLength = atoi(token);
             free(token);
             char* temp = NULL;
             readNbytes(clientfd, fileLength, NULL, &temp);
             getProjectVersion(temp, clientfd);
             free(temp);
             break;
			}else if(strcmp(mode, "sendFile") == 0){
				if(numOfFiles == 0){
					numOfFiles = atoi(token);
					
					file = (fileNode*) malloc(sizeof(fileNode) * 1);
					file->next = NULL;
					file->prev = NULL;
					
					free(token);
				}else if(fileName == 0){
					char* temp = NULL;
					fileLength = atoi(token);
					readNbytes(clientfd, fileLength, NULL, &temp);
					file->filepath = temp;
					
					char* name = (char*) malloc(sizeof(char) * strlen(basename(temp)) + 1);
					memset(name, '\0', sizeof(char) * strlen(basename(temp)) + 1);
					memcpy(name, basename(temp), strlen(basename(temp)));
					file->filename = name;
					
					free(token);
					fileName = 1;
				}else{
					file->filelength = (long long) atoi(token);
					insertLL(file);
					
					file = (fileNode*) malloc(sizeof(fileNode) * 1);
					file->next = NULL;
					file->prev = NULL;
					
					filesRead++;
					free(token);
					fileName = 0;
				}
				if(numOfFiles == filesRead){
					fileNode* temp = listOfFiles;
					while(temp != NULL){
						printf("File: %s\n", temp->filename);
						temp = temp->next;
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
			token[tokenpos] = buffer[0];
			tokenpos++;
		}
	}while(buffer[0] != '\0' && read != 0);
	pthread_exit(NULL);
}
void printFiles(){
	fileNode* temp = listOfFiles;
	while(temp != NULL){
		printf("File: %s with path of %s and length of %llu\n", temp->filename, temp->filepath, temp->filelength);
		temp = temp->next;
	}
}

/*
	Reads N bytes specified by length
		IF MODE = NULL: creates a token and stores into placeholder
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
		(*placeholder) = token;
	}
}

void createProject(char* directoryName, int clientfd){
	printf("Attempting to create the directory\n");
	int success = makeDirectory(directoryName);
	if(success){
		char* manifest = generateManifestPath(directoryName);
		int fd = open(manifest, O_WRONLY | O_TRUNC | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
		createManifest(fd, directoryName);
		close(fd);
		printf("Sending the Manifest to Client\n");
		writeToFile(clientfd, "SUCCESS");
		fileNode* manifestNode = createFileNode(manifest, strdup("Manifest"));
		sendFilesToClient(clientfd, manifestNode, 1);
		free(manifest);
	}else{
		printf("Directory Failed to Create: Sending Error to Client\n");
		writeToFile(clientfd, "FAILURE");
	}
}

void destroyProject(char* directoryName, int clientfd){
	int success = directoryTraverse(directoryName, 3, -1);
	int curDirectory = remove(directoryName);
	if(success == 1 && curDirectory != -1){
		printf("Server destroyed the project, sending client success message\n");
		writeToFile(clientfd, "SUCCESS");
	}else{
		printf("Server failed to destroy the project, sending error to client\n");
		writeToFile(clientfd, "FAILURE");
	}
}

void createManifest(int fd, char* directorypath){
	writeToFile(fd, "1");
	writeToFile(fd, "\n");
	directoryTraverse(directorypath, 0, fd);
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

/*
	Mode: 0 Read the Manifest and Record all the files as file objects and then read all the files. (Discard file version and hashcode)
*/
void readManifestFiles(char* projectName, int mode, int clientfd){
	char* manifest = generateManifestPath(projectName);
	numOfFiles = 0;
	int manifestfd = open(manifest, O_RDONLY);
	if(manifestfd == -1){
		printf("Error: Server does not contain the project or the Manifest is missing, sending error to client\n");
		writeToFile(clientfd, "FAILURE");
	}else{
		writeToFile(clientfd, "SUCCESS");
		printf("Searching the Manifest for the list of all files\n");
		numOfFiles++;
		fileNode* ManifestNode = createFileNode(manifest, strdup("Manifest"));
		insertLL(ManifestNode);
		
		char buffer[100] = {'\0'};
    	int defaultSize = 25;
    	char *token = malloc(sizeof(char) * (defaultSize + 1));
    	memset(token, '\0', sizeof(char) * (defaultSize + 1));
    
    	int read = 0;
    	int tokenpos = 0;
    	int bufferPos = 0;
    	int numOfSpaces = 0;
    
    	int manifestVersion = 0;
    
    	do{
        read = bufferFill(manifestfd, buffer, sizeof(buffer));
        for(bufferPos = 0; bufferPos < (sizeof(buffer)/sizeof(buffer[0])); ++bufferPos){
			  if(buffer[bufferPos] == '\n'){ 
		     		if(manifestVersion == 0){ //This is for version of the Manifest 
		     			manifestVersion = 1; 
		     		}else{
		     			int defaultSize = 25;
    					token = malloc(sizeof(char) * (defaultSize + 1));
   				 	memset(token, '\0', sizeof(char) * (defaultSize + 1));
   				 	tokenpos = 0;
		     			numOfSpaces = 0;
		     		}
		     }else if(buffer[bufferPos] == ' '){
		    		numOfSpaces++;
		    		if(numOfSpaces == 2){
		    			char* temp = strdup(token);
		    			char* filename = strdup(basename(temp));
		    			printf("The filepath is:%s\n",token);
		    			fileNode* newFile = createFileNode(token, filename);
		    			insertLL(newFile);
		    			numOfFiles++;
		    			free(temp);
		    		}
			  }else{
				if(manifestVersion != 0 && numOfSpaces == 1){
		    		if(tokenpos >= defaultSize){
						defaultSize = defaultSize * 2;
						token = doubleStringSize(token, defaultSize);
					}
					token[tokenpos] = buffer[bufferPos];
					tokenpos++;
		    	}
		    }
        }
		}while (buffer[0] != '\0' && read != 0);
		free(token);
		printf("The List of files in project is:\n");
	}
}

/*
	The Client will recieve 
		SUCCESS if the Manifest is found
		FAILURE if the Manifest is not found
	If SUCCESS: the server will create a long token that is the format of 
		NumberOfBytesOfToken$FileVersion\sFilepath\n
		Note the pattern will repeat for all files in the project and it will be one long continous token
*/
void getProjectVersion(char* directoryName, int clientfd) {
    printf("Attempting to get project Version\n");
    int fdManifest = open(strcat(directoryName, "/Manifest"), O_RDONLY);
    if(fdManifest == -1){
    	printf("Error: Project's Manifest Requested does not exist or does not have permissions to access on the server side, sending error to client\n");
    	writeToFile(clientfd, "FAILURE");
    	return;
    }else{
    	printf("Successfully found the project requested\n");
    	writeToFile(clientfd, "SUCCESS");
    }
    char buffer[50] = {'\0'};
    int defaultSize = 25;
    char *token = malloc(sizeof(char) * (defaultSize + 1));
    memset(token, '\0', sizeof(char) * (defaultSize + 1));
    
    int read = 0;
    int tokenpos = 0;
    int bufferPos = 0;
    int numOfSpaces = 0;
    
    int manifestVersion = 0;
    
    do {
        read = bufferFill(fdManifest, buffer, sizeof(buffer));
        for(bufferPos = 0; bufferPos < (sizeof(buffer)/sizeof(buffer[0])); ++bufferPos){
		     if(buffer[bufferPos] == '\n') { 
		     		if(manifestVersion == 0){ //This is for version of the Manifest (we don't want to send this to client)
		     			manifestVersion = 1; 
		     			free(token);
		     			
		     			tokenpos = 0;
		     			defaultSize = 25;
		     			char *token = malloc(sizeof(char) * (defaultSize + 1));
   					memset(token, '\0', sizeof(char) * (defaultSize + 1));
		     		}else{
		     			if(tokenpos >= defaultSize){ 
							defaultSize = defaultSize * 2;
							token = doubleStringSize(token, defaultSize);
						}
						token[tokenpos] = buffer[bufferPos];
						tokenpos++;
						numOfSpaces = 0;
		     		}
		     }else{
		     		if(buffer[bufferPos] == ' '){
		     			numOfSpaces++;
		     		}
		     		if(numOfSpaces < 2){
		     			if(tokenpos >= defaultSize){
							defaultSize = defaultSize * 2;
							token = doubleStringSize(token, defaultSize);
						}
						token[tokenpos] = buffer[bufferPos];
						tokenpos++;
		     		}
		     }
        }
    }while (buffer[0] != '\0' && read != 0);
    if(token[0] == '\0'){
    	printf("The project has no files\n");
    }
    printf("Sending the Client: %s\n", token);
    writeToFile(clientfd, "output$");
	 char str[3] = {'\0'};
	 sprintf(str, "%d", tokenpos);
	 writeToFile(clientfd, str);
	 writeToFile(clientfd, "$");
	 writeToFile(clientfd, token);
    free(token);
}

/*
Purpose: Setup the metadata of files
Format: numOfFiles$lengthofpathname$pathname$filesize
*/
void setupFileMetadata(int clientfd, fileNode* files, int numOfFiles){
	fileNode* file = files;
	char str[5] = {'\0'};
	sprintf(str, "%d", numOfFiles);
	writeToFile(clientfd, str);
	writeToFile(clientfd, "$");
	while(file != NULL){
		memset(str, '\0', sizeof(char) * 5);
		sprintf(str, "%lu", strlen(file->filepath));
		writeToFile(clientfd, str);
		writeToFile(clientfd, "$");
		writeToFile(clientfd, file->filepath);
		char filebytes[256] = {'\0'};
		sprintf(filebytes, "%lld", file->filelength);
		writeToFile(clientfd, filebytes);
		writeToFile(clientfd, "$");
		file = file->next;
	}
}

/*
Purpose: Send files to the Client
files must be a linked list of fileNodes
*/
void sendFilesToClient(int clientfd, fileNode* files, int numOfFiles){
	writeToFile(clientfd, "sendFile$");
	setupFileMetadata(clientfd, files, numOfFiles);
	fileNode* file = files;
	while(file != NULL){
		printf("File path: %s\n", file->filepath);
		if(file->filepath[0] == '.' && file->filepath[1] == '/'){
			sendFile(clientfd, file->filepath);
		}else{
			char* temp = (char*) malloc(sizeof(char) * strlen(file->filepath) + 3);
			memset(temp, '\0', sizeof(char) * strlen(file->filepath) + 3);
			temp[0] = '.';
			temp[1] = '/';
			memcpy(temp+2, file->filepath, strlen(file->filepath));
			printf("The new path is %s\n", temp);
			sendFile(clientfd, temp);
			free(temp);
		}
		
		file = file->next;
	}
}

/*
Purpose: Send one file to the Client
*/
void sendFile(int clientfd, char* filepath){
	int filefd = open(filepath, O_RDONLY);
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
	printf("Finished sending file: %s to client\n", filepath);
	close(filefd);
}

/*
Mode 0: Traverse through and add to manifest (the fd)
Mode 1: Traverse through the directories and create fileNodes (to be implemented)
Mode 2: Traverse through the directories and for each file, send to Client (Note* you should run MODE 1 to setup the Metadata of all these files) (to be implemented)
Mode 3: Traverse through the directories and delete all directories
*/
int directoryTraverse(char* path, int mode, int fd){ 
	DIR* dirPath = opendir(path);
	if(!dirPath){
		printf("Error: Directory path does not exist or no valid permissions!\n");
		return -1;
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
			if(mode == 0 && strcmp(curFile->d_name, "Manifest") != 0){
				char* hashcode = generateHashCode(filepath);
				char* temp = createManifestLine("1", filepath, hashcode, 0, 0);
				writeToFile(fd, temp);
				free(temp);
				free(hashcode);
			}else if(mode == 1 && strcmp(curFile->d_name, "Manifest") != 0){
				
			}else if(mode == 3){
				int success = remove(filepath);
				if(success == -1){
					printf("Error: File %s could not be deleted\n", filepath);
				}
			}
			free(filepath);
		}else if(curFile->d_type == DT_DIR){
			char* directorypath = pathCreator(path, curFile->d_name);
			if(mode == 3){
				directoryTraverse(directorypath, mode, fd);
				remove(directorypath);
			}else{
				directoryTraverse(directorypath, mode, fd);
			}
			free(directorypath);
		}else{
			
		}
		curFile = readdir(dirPath);
	}
	closedir(dirPath);
	return 1;
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


void appendToManifest(char* ProjectName, char* token){
	char* manifest = generateManifestPath(ProjectName);
	int fd = open(manifest, O_WRONLY | O_APPEND);
	if(fd == -1){
		printf("Fatal Error: Manifest was not found the project file\n");
		return;
	}
	writeToFile(fd, token);	
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


void writeToFile(int fd, char* data){
	int bytesToWrite = strlen(data);
	int bytesWritten = 0;
	int status = 0;
	while(bytesWritten < bytesToWrite){
		status = write(fd, (data + bytesWritten), (bytesToWrite - bytesWritten));
		if(status == -1){
			printf("Warning: write encountered an error\n");
			return;
		}
		bytesWritten += status;
	}
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

void calculateFileBytes(char* fileName, fileNode* file){
	struct stat fileinfo;
	bzero((char*)&fileinfo, sizeof(struct stat));
	int success = stat(fileName, &fileinfo);
	if(success == -1){
		printf("Error: File not found or lacking permissions to access file to get metadata\n");
	}else{
		file->filelength = (long long) fileinfo.st_size;
	}
}

fileNode* createFileNode(char* filepath, char* filename){
	fileNode* newNode = (fileNode*) malloc(sizeof(fileNode) * 1);
	newNode->filepath = filepath;
	newNode->filename = filename;
	calculateFileBytes(filepath, newNode);
	newNode->next = NULL;
	newNode->prev = NULL;
}


/*
	Given a path: it will generate all subdirectories for the given path (make sure the path ends with the directory and not a file)
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

char* pathCreator(char* path, char* name){
	char* newpath = (char *) malloc(sizeof(char) * (strlen(path) + strlen(name) + 2));
	memcpy(newpath, path, strlen(path));
	newpath[strlen(path)] = '/';
	memcpy((newpath + strlen(path) + 1), name, (strlen(name)));
	newpath[strlen(name) + strlen(path) + 1] = '\0';
	return newpath;
}

char* doubleStringSize(char* word, int newsize){
	char* expanded =  (char*) malloc(sizeof(char) * (newsize + 1));
	memset(expanded, '\0', (newsize+1) * sizeof(char));
	memcpy(expanded, word, strlen(word));
	free(word);
	return expanded;
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


void sighandler(int sig){
	exit(0);
}

void unloadMemory(){
	printf("Deallocing structures and closing file descriptors\n");
	while(listOfFiles != NULL){
		free(listOfFiles->filename);
		free(listOfFiles->filepath);
		fileNode* temp = listOfFiles;
		listOfFiles = listOfFiles->next;
		free(temp);
	}
	while(pthreadHead != NULL){
		pthread_join(pthreadHead->thread, NULL);
		userNode* temp = pthreadHead;
		pthreadHead = pthreadHead->next;
		free(temp);
	}
}
