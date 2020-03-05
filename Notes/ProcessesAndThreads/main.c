#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

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
     *         int pid = fork();
     *         if(pid == 0){
     *             execvp("./SomeExecutable", (char *const *) "argv[]");
     *         }
     *         wait(&pid);
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
     *     - Issues may occur with instruction interleaving
     * - Pros of Processes
     *   - If you want to load a new executable, you must use a process, threads cannot load new code
     *   - A separate way to handle signals/IO
     *     - If you want to handle signals/IO in 2 different ways you need 2 different processes
     *   - A separate address space (So other code cannot handle your data)
     *
     * Scenario: Multi-threaded application representing a bank, user deposits and withdraws in 2 threads running concurrently.
     * - We cannot simply mutli-thread it normally, because consistency will become an issue when these sets of instructions occur:
     *     Thread A			|	   Thread B
     *  load val to reg		|
	 *						|	load val to reg
	 *    incr reg			|
	 *   store to mm        |
	 *						|	   decr reg
	 *						|	 store to mm
     *                      |
     *                      |
	 *     Thread A			|	   Thread B
	 *  load val to reg     |
	 *    incr reg			|
	 *   store to mm        |
	 * 						|	load val to reg
	 * 						|	   decr reg
	 * 						|	 store to mm
     *                      |
     *                      |
	 *     Thread A			|	   Thread B
	 * 						|	load val to reg
	 * 						|	   decr reg
	 * 						|	 store to mm
	 *  load val to reg     |
	 *    incr reg			|
	 *   store to mm        |
     *
     * - Issue created because there are 3 operations to replace and a signal can occur between any 2 operations
     * - Possible Fixes?
     *   - Give the user control of scheduling: Dangerous
     *   - Give the user ability to be un-interruptable: Just as Dangerous
     *   - Use signals between the threads: Slow and somewhat difficult to resolve
     *   - Indicator variable: Also shared data, so the same issue can occur
     *
     * - MUTEX (Best Fix)
     *   - Primitive: test_and_set
     *   - Make indicator change a single instruction
     *     - So that it can't be interrupted (45) and is runnable instantly to indicate an event (45)
     *     - Make it a blocking call
     *   - We want to enforce MUTual EXclusion (MUTEX)
     *     - pthread_mutex_lock
     *     - pthread_mutex_unlock
     *   - Synchronization
     *     - critical section: segment of code that modifies/uses shared data
     *     - race condition: non-deterministic code that uses shared data whose results are based on run order
     *     - deadlock: synchronization that results in code that can never be run
     *   - ACID
     *     - A tomic
     *     - C onsistent
     *     - I solated
     *     - D urable
     *   - General Usage for MUTEX
     *     - Mutex must have full process scope because multiple threads will be using the same mutex
     *     - Wrap lock/unlock calls around critical sections
     *     - Keep critical sections as small as possible
     *     - Use only one mutex per shared data value
     *     - Naming Convention: mutex_<varname>_<counter>
     *   - MUTEX Ordering
     *     - Always resolved in lock order
     *     - All locked threads are removed from the run queue and added back in lock order
     *   - How to use
     *     - TO Create:
     *       - pthread_mutex_t alock;
     *       - pthread_mutex_init(&alock);
     *     - To Use:
     *       - pthread_mutex_lock(&alock);
     *       - pthread_mutex_unlock(&alock);
     *
     *   - Deadlocks
     *     - What Deadlocks need to exist:
     *         1. Mutual Exclusion (such as MUTEX)
     *         2. Hold and Wait
     *         3. No Preemption
     *         4. Circular wait: Mutexes that depend on each other to unlock
     *           - The only way to prevent deadlocks out of the four
     *     - Avoiding Deadlocks:
     *       - Maintain a global ordering on mutexes
     *         - Always lock in the same order to maintain a total ordering on the mutexes
     *       - Implement rising/falling permission phases
     *         - When you unlock, don't lock until you've unlocked ALL mutexes
     *     - Deadlock Example:
     *       - Both threads are waiting on each other to unlock their respective mutexes
     *      Thread A
     *    pthread_mutex_lock(&mutex_varA_0);
     *    pthread_mutex_lock(&mutex_varB_1); < Thread A waiting on the mutex that thread B holds
     *
     *      varA += 3*varB
     *
     *    pthread_mutex_unlock(&mutex_varA_0);
     *    pthread_mutex_unlock(&mutex_varB_1);
     *
     *    Thread B
     *    pthread_mutex_lock(&mutex_varB_1);
     *    pthread_mutex_lock(&mutex_varA_0); < Thread B waiting on the mutex that thread A holds
     *
     *    varB -= (varA+2);
     *
     *    pthread_mutex_unlock(&mutex_varB_1);
     *    pthread_mutex_unlock(&mutex_varA_0);
     *
     * pthread_join(...)
     * - Like waitpid() for processes
     *   - main wont return until all joins have returned
     *   - If you do not join, then your process may end before all threads are done running
     */

    return 0;
}
