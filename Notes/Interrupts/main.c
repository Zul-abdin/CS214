#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

// The Signal Handler
// This will continously loop when called by signal()
void aSignalHandler(int signum){
    printf("A segmentation fault occurred\n");
    exit(-1);
}

int main() {

    /*
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

    // Runs the function 'aSignalHandler' when SIGSEGV occurs (Segmentation Fault)
    signal(SIGSEGV, aSignalHandler);

    //Call kill on the running code to exit this loop, or uncomment the print
    while(1){

    }

    //A printf that causes a seg fault
    //printf("%d\n", *((int*)(0)));

    return 0;
}
