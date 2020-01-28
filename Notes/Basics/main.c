//System Library header files
#include <stdio.h>

//User-Created header files
#include "basicHeader.h"

//You can declare types here to allow full scope to variables, but it is better to put them in a header(.h) file.

int main(int argc, char* argv[]) {

    //A String is a null-terminated char array.
    //The \0 is the null-terminator which is an escape character that signifies end of a string
    char thisIsAString[] = {'s', 't', 'r', 'i', 'n', 'g', '\0'};

    //Enumerated type, the types are essentially just numbers
    //In C, if statements will run as long as the input is non-zero
    //Prefacing with '_' means it is a temp variable and is a note to not access it
    enum _boolean {true=1, false=0};

    //Promotes types or collections to top-level elements
    //Don't have to type enum for boolean now
    typedef enum _boolean boolean;

    typedef
    struct _stuff {
        int thing;
    }
    stuff;

    boolean fact = true;
    stuff somethingImportant;

    //A struct defines a single name for a collection of fields
    //Structs may have different memory sizes per element, unlike an array. Therefore you cannot reference them by indexing.
    //Stores each data type in a distinct chunk of memory
    struct thing {
        int anInt;
        char achar;
        double someDecimal;
    };

    //Different from struct in that it stores all data shared in one piece of memory
    //Changing one element changes all
    //Minimum size of union is the size of the largest data type
    //Used for pooling in large amounts data or interpreting the same bytes as differet types
    union diff {
        int anInt;
        char achar;
        double someDecimal;
    };

    //Initializing a struct takes values of variables left-right & top-bottom
    struct thing someStuff = {1, 'e', 1.49};
    someStuff.anInt = 4;

    printf("h%cllo %d \n", someStuff.achar ,someStuff.anInt);

    if(ISZERO(0)) {
        printf("%d\n", SQUARE(8+2));
    }
    return 0;
}
