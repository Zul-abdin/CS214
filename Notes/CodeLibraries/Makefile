# Makefiles:
# <Terget>: <Requirements>
#     <Terminal cmd>...
#
# Conventional Targets:
# - all: Builds everything
# - clean: Removes everything make built
# - test: Builds everything for testing and executes tests

all:	main compTests

main:	main.c plusOne.o plusTwo.o
	gcc main.c lib/plusOne.o lib/plusTwo.o -o bin/main.out

plusOne.o:	plusOne.c
	gcc -c plusOne.c -o lib/plusOne.o

plusTwo.o: plusTwo.c
	gcc -c plusTwo.c -o lib/plusTwo.o

clean:
	rm -rf bin/*
	rm -rf lib/*
	rm -rf testing/outputFiles/*

compTests:	testing/testCode/test0.c testing/testCode/test1.c
	gcc testing/testCode/test0.c lib/plusOne.o -o bin/test0.out
	gcc testing/testCode/test1.c lib/plusOne.o -o bin/test1.out

test:	test0 test1
	echo "done testing"

test0:	testing/testCode/test0.c lib/plusOne.o bin/test0.out
	./bin/test0.out > testing/outputFiles/Out_0; diff testing/outputFiles/Out0 testing/testCaseResults/Result_0 | wc

test1:	testing/testCode/test1.c lib/plusOne.o bin/test1.out
	./bin/test1.out > testing/outputFiles/Out_1; diff testing/outputFiles/Out1 testing/testCaseResults/Result_1 | wc

everything:	clean all test