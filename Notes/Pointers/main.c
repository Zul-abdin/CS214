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
     * - A memory address (of something whose size is known)
     *   - *n: dereference n (Treat contents of n as a memory address and go to that location, getting the value there)
     *   - &n: gets address of n
     */

    char test0[] = "HELLO";
    char test1[] = "hello";

    char* p = &(test1[0]);

    printf("test0 -> %p\n", test0);
    printf("%p + %lu = %p\n", test0, 3 * sizeof(char), (test0 + (3 * sizeof(char))));
    printf("%p -> %c\n", (test0 + (3 * sizeof(char))), *(test0 + (3 * sizeof(char))));
    printf("test0[3] = %c\n", test0[3]);

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
     * Malloc
     * - Returns the address of start of allocated memory (void pointer type), and NULL if unsuccessful
     * - Need to cast malloc-ed memory so that it's type is known
     * - Need to free(...) afterwards
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

    return 0;
}
