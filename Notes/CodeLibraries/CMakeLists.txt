cmake_minimum_required(VERSION 3.15)
project(CodeLibraries C)

set(CMAKE_C_STANDARD 90)

add_executable(CodeLibraries plusOne.c include/plusOne.h testing/testCode/test0.c main.c testing/testCode/test1.c include/plusTwo.h plusTwo.c)