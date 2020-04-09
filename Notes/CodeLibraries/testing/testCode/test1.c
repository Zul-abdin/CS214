//
// Created by Zain on 3/26/2020.
//

#include <stdio.h>
#include "../../include/plusOne.h"

int main(){

    /*
     * Libraries
     * - Implimentations of functions that can be included into executables
     * - Has no main
     * - May include in to it
     * - Has implementations of functions
     * - Has an associated header file
     *   - Header file has prototype of all the library's functions
     *
     * - Using Libraries
     *   - Include the header in your executable code
     *   - compile the library: gcc -c libratyname.c -> libraryname.o
     *   - Hand libraries to gcc directly: gcc executablesrc.c libraryname.o
     */

    printf("%d", plusone(22));

    return 0;
}