#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char* argv[]) {

    /*
     * IO
     * - Is often memory-mapped
     *   - i.e. some virtual address 'means' the device
     * - Non-blocking vs blocking Requests
     *   - Blocking: the next line will only run when when the function returns
     *   - Non-blocking: next line may run after a partial run of the function
     *     - Resumes running your code when you are scheduled regardless of the progress of the non-blocking function
     *   - IO is so slow that they are sometimes it uses Non-blocking requests
     *     - Therefore you should ALWAYS check the return value of IO functions you use to check for partial successes
     * - Handling Errors
     *   - errno (in <errno.h>)
     *     - A global error number that lets you to react to errors by getting detailed description of errors
     *     - Will also indicate success
     *   - Check errno immediately after the error
     *   - Can use perror in terminal or strerror in your code
     *
     * File Descriptor
     * - An IO "pointer"
     *   - Memory mapped, a memory address that references some outside file/data/device, but the memory address is not real
     *   - An abstraction to represent something outside your code you can read/write from/to
     * - Used to:
     *   - Open, Close, Read, Write
     *     - open(path, flags): Used to get a file descriptor for some device
     *       - flags are opening modes. eg. only read mode, read/write mode, etc.
     *     - close(file descriptor): Used to close a file descriptor when you are done using it
     *     - read(...) and write(...): Used to read/write from/to a file descriptor
     *     - To check man pages, look in chapter 2 or 3 by doing:
     *       - man 2 read
     *   - f functions (fopen, fclose, ...)
     *     - Used only for low latency, high volume data
     *     - Do not use them for other types of data and most types of data in this class
     *
     *
     * Read(int fd, void *buff, size_t count)
     * - Attempts to read up to count bytes from the file descriptor, fd, into the buffer, *buff
     * - in <unistd.h> library
     * - Returns:
     *   - Number of bytes read if successful
     *     - This means that if you want to read 100 bytes it might return less than 100 bytes because it wasnt able to read the rest
     *   - 0 if you reached the EOF (end of file)
     *
     * File Path
     * - ".." to go back one directory
     * - start with a "/" to signify an absolute path
     *
     * File Storage & Directories
     * - HDD only stores bytes, in disk blocks
     * - But we need persistent storage to store the location of dick blocks and files
     *   - Such metadata is stored in a special section of the HDD
     * - Data needed to access files stored in "i-nodes"
     *   - Made up of metadata and pointers
     *   - Meta-data for the disk blocks
     *   - Info needed to access a file
     *     - List of disk blocks that make up the file
     *     - Access mode of file
     *     - Info about the file (eg. File name, creation date, last used, ...)
     *   - Meta-data for files is the same as the meta-data for directories
     *     - The only difference is what the i-nodes will point to
     *       - In the case of files, the i-nodes have pointers to disk blocks
     *       - in the case of directories, the i-nodes have pointers to other i-nodes
     *     - So i-nodes can implement both files AND directories
     *   - Use pointers to allow for multiple sized files
     *
     */

    printf("~~~~~~~~~~~~~~~~~~~~~~~~\n");

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    //Initializing file descriptor in read-only mode
    int fd = open("./main.c", O_RDONLY);

    //Check if initialization worked
    if(fd < 0){
        printf("Initializing fd failed\n");
        return -1;
    }

    char buffer = '?';

    //Read one byte from fd("./main.c"), into buffer and check if it's valid, if so print it
    while(read(fd, &buffer, 1) > 0) {
        printf("%c", buffer);
    }
    printf("\n");

    printf("~~~~~~~~~~~~~~~~~~~~~~~~\n");

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    int fd1 = open("./main.c", O_RDONLY);

    //Making a buffer for a 100-character string, initially setting everything to the null-terminator

    char buffer1[101];
    memset(buffer1, '\0', 101);

    int status;
    int readIn = 0;

    do{
        status = read(fd1, buffer1 + readIn, 100 - readIn);
        readIn += status;

    } while(status > 0 && readIn < 100);

    printf("%s", buffer1);
    printf("%c", '\n');


    printf("~~~~~~~~~~~~~~~~~~~~~~~~\n");

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    return 0;
}
