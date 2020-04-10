#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>

void writeToFile(int fd, char* data);
char* doubleStringSize(char* word, int newsize);
char** getConfig();
void bufferFill(int fd, char* buffer, int bytesToRead);
int connectToServer(int socketfd, char** serverinfo);
int createSocket();

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
    			
    		}else if(strlen(argv[1]) == 7 && strcmp(argv[1], "destroy") == 0){ //destroy
    			
    		}else if(strlen(argv[1]) == 14 && strcmp(argv[1], "currentversion") == 0){ //currentversion
    			char** information = getConfig();
    			if(information != NULL){
    				int socketfd = createSocket();
    				connectToServer(socketfd, information);
    			}
    			
    		}else if(strlen(argv[1]) == 7 && strcmp(argv[1], "history") == 0){ //history
    			
    		}else{
    			printf("Invalid operation\n");
    		}
    		
    	}else{
    		if(strlen(argv[1]) == 9 && strcmp(argv[1], "configure") == 0){ //Configure
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
	if(connect(socketfd, (struct sockaddr*)&serverinfostruct, sizeof(serverinfostruct)) < 0){
		printf("Error: Cannot connect\n");
		return -1;
	}
	return 1;
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
		printf("Fatal Error: Configure file does not exist\n");
		return NULL;
	}
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
		bufferFill(fd, buffer, sizeof(buffer));
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
	}while(buffer[0] != '\0' && finished == 0);
	return information;
}

char* doubleStringSize(char* word, int newsize){
	char* expanded =  (char*) malloc(sizeof(char) * (newsize + 1));
	memset(expanded, '\0', (newsize+1) * sizeof(char));
	memcpy(expanded, word, strlen(word));
	free(word);
	return expanded;
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

