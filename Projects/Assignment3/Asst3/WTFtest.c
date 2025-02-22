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
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
int bufferFill(int fd, char* buffer, int bytesToRead);
void checkResult(char* resultpath, char* result, char* testcase);
void generateResult(char* command);

int main(int argc, char** argv) {
	char* serverpath = "./serverFolder/WTFserver";
	char* clientpath = "./clientFolder/WTF";
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
	
	char* resultpath = "result.txt";;
	generateResult("./clientFolder/WTF create project1 2 > result.txt");
	checkResult(resultpath, "Fatal Error: Invalid operation or Improperly Formatted", "Testcase 0");
	
	generateResult("./clientFolder/WTF update 1 2 3 4 > result.txt");
	checkResult(resultpath, "Fatal Error: Incorrect number of arguments.", "Testcase 1");
	
	generateResult("./clientFolder/WTF configure > result.txt");
	checkResult(resultpath, "Fatal Error: Incorrect number of arguments.", "Testcase 2");
	
	generateResult("./clientFolder/WTF add 1 2 > result.txt");
	checkResult(resultpath, "Fatal Error: Project does not exist to add the file", "Testcase 3");
	
	generateResult("./clientFolder/WTF remove 1 2 > result.txt");
	checkResult(resultpath, "Fatal Error: Project does not exist to remove the file", "Testcase 4");

	generateResult("./clientFolder/WTF create project1 > result.txt");
	checkResult(resultpath, "Fatal Error: Configure file is missing or no permissions to Configure file, please call configure before running", "Testcase 5");
	
	printf("Forking to create the server and client\n");
	int childpid;
	childpid = fork();
	if(childpid == 0){
		chdir("./serverFolder");
		printf("The directory has changed to server's folder\n");
		char* argv_list[] = {"./WTFserver", "9123", NULL};
		execv("./WTFserver", argv_list);
		//generateResult("./serverFolder/server 9123");
	}else if(childpid > 0){
		generateResult("cd ./clientFolder; ./WTF configure localhost 9123; ./WTF create project1; ./WTF history project1 > result.txt");
		char* clientpath = "./clientFolder/result.txt";
		checkResult(clientpath, "Successful Connection to Server\nWarning: History file is empty, nothing to output\nClient: Terminated Server Connection\n", "Testcase 6");
		kill(childpid, SIGKILL);
		wait();
	}else{
		printf("Fork could not be done, not enough threads\n");	
	}
	wait();
	
	//generateResult("./clientFolder/client add 1 2 > result.txt");
	//checkResult(resultpath, "Fatal Error: Project does not exist to add the file", "Testcase 7");
	
	return 0;
}

void generateResult(char* command){
	system(command);
}

void checkResult(char* resultpath, char* result, char* testcase){
	int resultfd = open(resultpath, O_RDONLY);
	if(resultfd == -1){
		printf("Something went wrong with this testcase\n");
	}else{
		char buffer[500] = {'\0'};
		bufferFill(resultfd, buffer, strlen(result));
		if(strcmp(buffer, result) == 0){
			printf("%s Passed\n", testcase);
		}else{
			printf("%s failed\n", testcase);
			printf("Your result was:%s", buffer);
		}
		close(resultfd);
	}
	remove(resultpath);
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

