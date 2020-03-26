//
// Created by Zain on 1/23/20.
//
/*
 * Header Files
 * - Text Replacement Mechanisms (ex. Will not evaluate subexpressions within macro inputs)
 *   > Definitions (#define) enum, struct, union, typedef
 *   > Macros - Definitions with variables
 *
 * - Conventions
 *   > all defs in caps
 *   > ifndef guard at top of file
 */

// ifndef Guard - prevents running/processing your header files more than once
#ifndef _BASICHEADER_H //if "_BASICHEADER_H" not defined (not existing)
#define _BASICHEADER_H //define it

//Name definitions in header files (Macros) in caps
//Not a variable, cannot be changed
//Binds the value 8 to SOMEVALUE
#define SOMEVALUE 8

//Not a function, just text replacement
#define ISZERO(x) x==0
#define SQUARE(x) x*x

enum typeDefinedInHeader {type1=10, type2=20};

#endif //_BASICHEADER_H