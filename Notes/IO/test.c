//
// Created by Zain on 2/11/20.
//

// A testing file for my own learning

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <zconf.h>

int main(int argc, char* argv[]){
    printf("testing...\n");

    int fd = open("./testfile.txt", O_RDONLY);

    char buffer[4];

    while(read(fd, &buffer, 4 * sizeof(char)) > 0) {
        printf("%s", buffer);
    }

    return 0;
}