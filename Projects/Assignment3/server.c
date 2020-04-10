#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
int port;

int main(int argc, char** argv){
	if(argc != 2){
		printf("Fatal Error: invalid number of arguments\n");
	}else{
		port = argv[1]; //Assuming argv[1] is always a number currently
	}
	return 0;
}
