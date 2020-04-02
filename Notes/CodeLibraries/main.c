//
// Created by Zain on 3/26/2020.
//

#include <stdio.h>
#include "include/plusOne.h"

int main(){

    /*
     * Archives
     * - A library of libraries
     * - ar <flag> lib<NAME>.a <files>
     *   - flags:
     *     -R (Replace): replace an object file/library in an archive
     *     -D (Delete): remove an object file/library from an archive
     *     -T (Table): list of all object files/library in an archive
     */

    printf("%d\n", plusone(99));

    return 0;
}