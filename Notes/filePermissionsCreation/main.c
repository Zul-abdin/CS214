#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>


int main() {

    /*
     * File Permissions: The most restrictive classifications are always used
     *  ls -alF <fileName>: check permissions
     *
     * - Permissions
     *   - read
     *   - write: Includes deleting files
     *   - execute
     *
     * - Groups
     *   - user(owner): Can change permissions on the file
     *   - group(set of users)
     *   - other(not owner and not in file's group)
     *   - "all" - hidden group
     *
     *  u   g   o
     * rwx rwx rwx
     *
     * - chmod: Used to alter permissions
     *   - chmod <classification>+/-<permission> <file>
     *   - Examples
     *     - chmod g+w test.c
     *     - chmod a-r test.c
     *     - chmod o+x test.c
     *   - chmod <tripleOctet> <file>
     *     - Lets say you cant the following permissions for test.c:
     *            u   g   o
     *           rwx rwx rwx
     *           110 001 101 --> 6  1  5
     *     - Just convert the binary to a positive integer, and use it as posBitStringRepresentation
     *       - chmod 615 test.c
     *
     * File Comparison
     * - Difference: diff <fileName> <fileName>: check if two files are identical or different
     *   - Prints the lines that are different
     * - Difference piped into Word Count: diff <fileName> <fileName> | wc: tells you how many words are different
     *   - diff of two files piped into word count (wc)
     * - Octal Dump: od <flag: -c/-d/...> <fileName>: outputs bytes of a file with different formattings
     *   - the <-c> tells od to print actual character encodings, such as '\t' and '\n'
     *
     * File Viewing
     * - cat <fileName> <fileName> <fileName> <fileName> ...: Used to concatenate files into one file and print it to STDOUT
     *   - Usually used to view a file using: cat <fileName>
     * - more <fileName> <fileName> <fileName> <fileName>...: Used to display one screen-worth of data at a time
     *   - Press space to advance 1 screen length
     *   - Press return to advance one line
     * - less <fileName> <fileName> <fileName> <fileName>...: more, but with extra goodies
     *   - Use up/down arrows to move across the file
     *   - /<String> to search
     *   - Use 'q' to quit viewing the file
     *
     */

    int fd = open("./exampleFile", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    close(fd);

    return 0;
}
