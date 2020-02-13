#include <zconf.h>
#include <sys/wait.h>

int main() {

    /*
     * A representation of running code in a Machine (All created when calling fork)
     * - The following loaded into main memory (DRAM)
     *   - Instructions (actual code)
     *   - Stack
     *     - Sequence of function calls
     *     - Top is the activation record (currently active context)
     *   - Heap
     *     - Dynamic memory allocations at runtime
     *   - Global
     *     - Visible throughout all your code (can be statically allocated)
     *   - name/number
     *   - Metadata
     *
     * fork(void)
     * - duplicates the current process as the representation of running code
     * - Creates a child process
     * - The new stack is exactly the same, along with everything tht was copied
     * - Reformat a process and load different code
     *   - With fork, all you get is an exact copy of the current state of the code
     *   - exec(code)
     *     - executes a file
     *     - Using exec on main code itself will cause the child process to run your main code, and the parent to run the file code.
     *       - Not ideal
     * - Returns 0 for the child process, such that the child process can be identified and you can make the child use exec(...)
     *
     * Completing a Child Process
     * - wait(NULL)
     *   - Returns when the first child process has finished running
     * - waitpid(pid)
     *   - Returns when a specific child process has finished running
     */

    int pid = fork();

    if(pid == 0){
        execvp("./SomeExecutable", (char *const *) "argv[]");
    }

    wait(&pid);

    return 0;
}
