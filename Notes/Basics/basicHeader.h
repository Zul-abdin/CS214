//
// Created by Zain on 1/23/20.
//

#ifndef _BASICHEADER_H //if not defined (not included)
#define _BASICHEADER_H //define and include it

#endif //_BASICHEADER_H

//Name definitions in header files (Macros) in caps
//Not a variable, cannot be changed
//Binds the value 8 to SOMEVALUE
#define SOMEVALUE 8

//Not a function, just text replacement
#define ISZERO(x) x==0
#define SQUARE(x) x*x

enum typeDefinedInHeader {type1=10, type2=20};