#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

// The Signal Handler
// This will continously loop when called by signal()
void aSignalHandler(int signum){
    printf("A segmentation fault occurred\n");
    exit(-1);
}

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
     * - A process is the OS's abstraction for running code
     *
     * fork(void)
     * - duplicates the current process as the representation of running code
     * - Creates a child process
     * - The new stack is exactly the same, along with everything tht was copied
     * - Reformat a process and load different code
     *   - With fork, all you get is an exact copy of the current state of the code
     *   - exec(binary)
     *     - executes an already compiled binary
     *     - loads new instructions
     *     - 'flushes' out internal structures and instructions
     *       - reallocate all internal structures for new code
     *     - Using exec on main code itself will cause the child process to run your main code, and the parent to run the file code.
     *       - Not ideal
     *     - Exec only returns on error, because if exec is successful, it's stack becomes empty and it can't return
     * - Returns 0 for the child process, such that the child process can be identified and you can make the child use exec(...)
     * - Returns PID (strictly positive) to parent process
     * - Fork Bombs
     *   - When child processes also call fork, creating an exponential increase of processes
     *   - Prevent them with: ulimit -u, to set a max number of processes
     *
     * Completing a Child Process
     * - wait(NULL)
     *   - Returns when the first child process has finished running
     *   - Basically BLOCKS future code until any one child process ends
     *   - Informs OS that the process is done and it can be reaped
     *     - The process is de-allocated
     * - wait_pid(pid)
     *   - Returns when a specific child process has finished running
     *
     * Process Cleanup Issues
     * - Orphan Process
     *   - Processes whose parents are no longer in the system
     *
     * - Zombie Process
     *   - Processes with no runnable code
     *   - Process has returned
     *   - Process has posted it's exit signal
     *   - However, the parent has not had wait() called on them, and therefore cannot be de-allocated
     *
     * - Zombie Orphans
     *   - Can detected by OS scheduler and removed
     *     - Do not depend on this, you should always call wait on such processes
     *
     * Interruptions
     * - Exception
     *   - Specify a region where the exception can occur (try block)
     *   - Can catch: Specify code to run on exception
     *   - On exception, jump to second code block and continue
     * - Signals: Software interrupt to deal with something important
     *   - Example: Re-sizing terminal window while code is executing
     *   - Can occur anywhere, anytime, between any two instructions, don't need to specify a region
     *   - Can catch, although it is only a request to the OS to run the code to handle the signal
     *   - On signal,
     *     - It will check if you have a signal handler:
     *       - If you have a signal handler creates a new stack frame at top of stack for it, if it decides to run it
     *       - Else, runs the default code from OS
     *     - After the signal handler returns, the OS will restore previous context and begin to run  exactly where you left off
     *   - signal(int signum, void(*func)(int) FunctionPointer) in signal.h
     *     - Returns a signal handler that will run the code given when the signal, signum, occurs
     *   - How to send a signal to any PID:
     *     - ps aux | grep bin/main (Finds every line with bin/main)
     *     - Call kill -s <Name of signal eg. SIGSEGV> <PID>
     * - Signal vs Exception
     *   - Exceptions will skip over all the code in the try-catch
     *   - Signals will always resume at the point where the signal occurs
     *
     * Threads: A new context of execution running in the same process
     * - A new stack allocated in the heap to hold the new context
     * - pthread.h library
     *   - pthread_create(pthread_t* thread, const pthread_attr_t* attr, void* (*start)(void*), void* arg)
     *     - pthread_t* thread: Handle for the currently-created thread. It represents the thread.
     *     - const pthread_attr_t* attr: struct of pthread options (Can usually use the default one)
     *     - void* (*start)(void*): function to run as a thread
     *     - void* arg thread function's parameters (If you have multiple arguments to pass, use a struct ON THE HEAP)
     * - Pros of Threads
     *   - It is an independent stack in your current process
     *   - It is faster than creating processes
     *   - It is faster than switching processes
     *   - It is easier to pass data between threads because it shares the same heap
     * - Pros of Processes
     *   - If you want to load a new executable, you must use a process, threads cannot load new code
     *   - A separate way to handle signals/IO
     *     - If you want to handle signals/IO in 2 different ways you need 2 different processes
     *   - A separate address space (So other code cannot handle your data)
     *
     * Exiting Code
     * - return
     *   - Throws away the current stack frame
     *   - Posts exit signal
     * - exit()
     *   - Throws away the entire current stack. If that is the only stack, it will stop the process
     *   - Posts exit signal
     * - _exit()
     *   - Stops the current process as fast as possible
     *   - Does not post the exit signal
     * - atexit(void (*func)(void))
     *   - runs code after the exit signal has been posted
     *   - good for post-run cleanup like de-allocating heap and memory
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

/*
    int pid = fork();

    if(pid == 0){
        execvp("./SomeExecutable", (char *const *) "argv[]");
    }

    wait(&pid);
*/

    // Runs the function 'aSignalHandler' when SIGSEGV occurs (Segmentation Fault)
    signal(SIGSEGV, aSignalHandler);

    //Call kill on the running code to exit this loop, or uncomment the print
    while(1){

    }

    //A printf that causes a seg fault
    //printf("%d\n", *((int*)(0)));

    return 0;
}
