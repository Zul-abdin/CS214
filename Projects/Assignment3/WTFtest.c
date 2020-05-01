#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <libgen.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
int bufferFill(int fd, char* buffer, int bytesToRead);

int main(int argc, char** argv) {
	char* serverpath = "./serverFolder/server";
	char* clientpath = "./clientFolder/client";
	int serverfd = open(serverpath, O_RDONLY);
	if(serverfd == -1){
		printf("The server folder has not been created, please run \"make test\" ");
		return 0;
	}
	close(serverfd);
	int clientfd = open(clientpath, O_RDONLY);
	if(clientfd == -1){
		printf("The client folder has not been created, please run \"make test\" ");
		return 0;
	}
	close(clientfd);
	
	printf("Running Testcases 0\n");
	
	char* resultpath = "./result.txt";;
	char* test0 = "./clientFolder/client create project1 2 3 > result.txt";
	char* result0 = "Fatal Error: Invalid operation or Improperly Formatted";
	system(test0);
	system(result0);
	int resultfd = open(resultpath, O_RDONLY);
	if(resultfd == -1){
		printf("Something went wrong with this testcase\n");
	}else{
		char buffer[150] = {'\0'};
		bufferFill(resultfd, buffer, strlen(result0));
		int status = read(resultfd, buffer, strlen(result0));
		if(strcmp(
		close(resultfd);
	}
	return 0;
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

