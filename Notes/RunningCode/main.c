#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {

    /*
     * Process: A representation of running code in a Machine (All created when calling fork)
     * - The following loaded into main memory (DRAM)
     *   - Instructions (actual code)
     *   - Stack
     *     - Sequence of function calls
     *     - Top is the activation record (currently active context) and then starts another context for the function
     *   - Heap
     *     - Dynamic memory allocations at runtime
     *   - Global
     *     - Visible throughout all your code (can be statically allocated)
     *     - Useful for the OS, to be used in processes for things like errno
     *   - identifier (PID, PPID)
     *     - Each process needs to keep track of its own ID and it's parent process's ID
     *   - Metadata (BSS region)
     *     - OS needs to record management info about your process
     * - A process is basically a struct, with the info from above
     * - An OS's abstraction for running code
     * - A process is the code currently running
     * - A process is basically a struct, with the info from above
     *
     * fork(void)
     * - duplicates the current process as the representation of running code
     * - Creates a child process
     * - The new stack is exactly the same, along with everything tht was copied
     * - Reformat a process and load different code
     *   - With fork, all you get is an exact copy of the current state of the code
     *   - exec(binary)
     *     - executes an already compiled binary
     *     - loads ne instructions
     *     - reallocate all internal structures for new code
     *     - Using exec on main code itself will cause the child process to run your main code, and the parent to run the file code.
     *       - Not ideal
     *     - Exec only returns on error, because if exec is successful, it's stack becomes empty and it can't return
     * - Returns 0 for the child process, such that the child process can be identified and you can make the child use exec(...)
     * - Returns PID (strictly positive) to parent process
     *
     * Completing a Child Process
     * - wait(NULL)
     *   - Returns when the first child process has finished running
     * - waitpid(pid)
     *   - Returns when a specific child process has finished running
     * - A process can never be de-allocated, it can only be stopped
     *
     * Process Cleanup Issues
     * - Orphan Process
     *   - Processes whose parents are no longer in the system
     *
     * - Zombie Process
     *   - Processes with no runnable code left that have not had wait() called on them
     *
     * - Zombie Orphans
     *   - Can detected by OS scheduler and removed
     *     - Do not depend on this, you should always call wait on such processes
     *
     * Signals
     * - Exception
     *   - Specify a region where the exception can occur
     *   - Can catch: Specify code to run on exception
     *   - On exception, jump to second code block and continue
     * - Signal
     *   - Can occur anywhere, anytime, between any two instructions, don't need to specify a region
     *   - Can catch, although it is only a request to the OS to run the code to handle the signal
     *   - On signal,
     *     - It will check if you have a signal handler:
     *       - If you have a signal handler creates a new stack frame at top of stack for it, if it decides to run it
     *       - Else, runs the default code from OS
     *     - After the signal handler returns, the OS will restore previous context and begin to run  exactly where you left off
     */

    /*
     * Exam Details
     *
     * Question Types
     *  Description/Identification
     *      ex)What is blocking?
     *      ex)What is malloc, what does it do?
     *      - Make sure you know all methods, how to use them, and their return types
     *  Function
     *      ex)How to make read work if it doesnt give you all the bytes?
     *      ex)How can malloc fail?
     *      - Make sure you know how all methods can fail, or give unexpected results
     *
     *  Operation
     *      ex)If this happens, what would you do to make the function work?
     *      ex)How would you utilize malloc to do ...
     *      - Make sure you can tell how a method failed, and when a method should be used and how its result should be interpreted
     */

    int pid = fork();

    if(pid == 0){
        execvp("./SomeExecutable", (char *const *) "argv[]");
    }

    wait(&pid);

    return 0;
}
