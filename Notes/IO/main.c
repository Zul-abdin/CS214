#include <stdio.h>

int main() {

    /*
     * IO
     * - Is often memory-mapped
     *   - i.e. some virtual address 'means' the device
     * - Non-blocking vs blocking Requests
     *   - Blocking: the next line will only run when when the function returns
     *   - Non-blocking: next line may run after a partial run of the function
     *   - IO is so slow that they are sometimes it uses Non-blocking requests
     *     - Therefore you should ALWAYS check the return value of IO functions you use to check for partial successes
     * - Handling Errors
     *   - Check errno immediately after the error
     *   - Can use perror in terminal or strerror in your code
     *
     * File Descriptor
     * - An IO pointer
     *   - An abstraction to represent something outside your code you can read/write from/to
     * - Used to:
     *   - Open, Close, Read, Write
     *     - To check man pages, look in chapter 2 or 3 by doing:
     *       - man 2 read
     *
     * Read(int fd, void *buff, size_t count)
     * - Attempts to read up to count bytes from the file descriptor, fd, into the buffer, *buff
     * - in <unistd.h> library
     * - Returns:
     *   - Number of bytes read if successful
     *     - This means that if you want to read 100 bytes it might return less than 100 bytes because it wasnt able to read the rest
     *   - 0 if you reached the EOF (end of file)
     */

    printf("~~~~~~~~~~~~~~~~~~~~~~~~\n");

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


    return 0;
}
