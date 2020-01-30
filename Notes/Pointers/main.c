#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {

    /*
     * Arrays (assume an array named arr)
     * - Arrays are stored in sequential order in memory
     * - To mechanically index through an array, to get the element in an index,
     *   - do: index * sizeof(type of arr) to get how far in memory from the first element that index is
     * - The name of the array is a pointer to the address of the first element in the array in memory, whether it be a[0], a[0][0], ...
     * - So, doing: arr + (index * sizeof(type of array)) gives memory location of that index, because arr is a pointer to an address
     *   - You can also do: arr + (index), because the above is done automatically.
     * - For a 1-D array: arr IS a pointer, but arr[0] is NOT
     */
    char thisIsAString[] = {'s', 't', 'r', 'i', 'n', 'g', '\0'};

    //These both reference the same element
    // (*) dereferences a memory address
    // (&) gives the memory address of an element
    printf("%c\n",thisIsAString[3]);
    printf("%c\n", *(thisIsAString + 3));

    //These print the same memory address
    printf("%p\n",&thisIsAString[3]);
    printf("%p\n",thisIsAString + 3);

    printf("~~~~~~~~~~~~~~~~~~~~~~~~\n");

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /*
     * Pointers
     * - Holds a memory address (of something whose size is known)
     *   - *n: dereference n (Treat contents of n as a memory address and go to that location, getting the value there)
     *   - &n: gets address of n
     *   - incrementing a pointer increments the memory value by a factor of the sizeof(type of pointer)
     */

    char test0[] = "HELLO";
    char test1[] = "hello";

    char* p = &(test1[0]);

    printf("test0 -> %p\n", test0);
    printf("%p + %lu = %p\n", test0, 3 * sizeof(char), (test0 + (3 * sizeof(char))));
    printf("%p -> %c\n", (test0 + (3 * sizeof(char))), *(test0 + (3 * sizeof(char))));
    printf("test0[3] = %c\n\n", test0[3]);

    int test2[] = {1, 2, 3, 4, 5, 6, 7, 8};
    printf("For int arrays:\n");
    printf("test2 -> %p\n", test2);

    //Adding 3 to a int pointer actually adds 3*typeof(int)
    printf("%p + 3 = %p\n", test2, (test2 + 3));
    printf("%p -> %d\n", (test2 + (3)), *(test2 + 3));
    printf("test[3] = %d\n", test2[3]);

    //Print what p is pointing to
    printf("%c\n", *p);

    //Set the value at pointer p equal to 'f'
    //Common error: p = 'f'; (Makes the address p be 'f', treating 'f' as an address rather than a char)
    *p = 'f';

    printf("%c\n", test1[0]);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~\n");

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    //Dynamically Allocated Memmory
    /*
     * void* Malloc (<How much memory you want>)
     * - Returns the address of start of allocated memory (void pointer type {void*}), and NULL if unsuccessful
     * - Need to cast malloc-ed memory so that it's type is known
     * - Need to free(...) afterwards
     * - man malloc (manual pages) in terminal for more info
     *
     * void* Memcpy (<dest pointer>, <src pointer>, <size of memory to copy>)
     *
     * void* Realloc (<Old pointer>, <new size>)
     *  - A script that does: malloc, memcpy, free
     *  - Unreliable, and best to do it manually
     *
     *  void* Calloc
     *  - assigns memory to physical bytes instead of virtual memory from the OS
     */

    //Linked List node
    struct _Node{
        int value;
        struct _Node* next;
    };
    typedef struct _Node Node;

    Node LLhead;
    LLhead.value=22;
    LLhead.next = (Node*)malloc(sizeof(Node));
    LLhead.next->value = 23;
    LLhead.next->next = NULL;

    printf("%d\n", LLhead.next->value);

    free(LLhead.next);

    printf("~~~~~~~~~~~~~~~~~~~~~~~~\n");

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    //4 bytes of memory given to a
    int a = 0;

    //Making a char pointer and setting it to the address of a, treated as a char pointer
    char* po = (char*) &a;

    //4 chars can fit into an int, so this should work
    //Remember - po points to the address of a, casted to a char array (char pointer)
    po[0] = 'h';
    po[1] = 'i';
    po[2] = '!';
    po[3] = '\0';

    printf("%s\n", po);

    return 0;
}
