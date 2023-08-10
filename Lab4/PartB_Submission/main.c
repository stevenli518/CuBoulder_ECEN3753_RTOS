#include <stdio.h>

#define CTEST_MAIN
#define CTEST_SEGFAULT
#define CTEST_COLOR_OK

#include "ctest.h"

//----------------------------------------------------------------------------------------------------------------------------------
/// @Instructions
/// 1. Complete implementation of queue.c
/// 2. Complete implementation of rr.c
/// 3. Complete implementation of rrtests.c
/// 4. Read the .h files very carefully
/// 5. The quantim time in round robin will be passed in as a parameter
///
/// @Note
/// 1. You cannot modify queue.h
/// 2. You cannot modify rr.h
/// 3. You cannot modify main.c
///
/// @Makefile
/// 1. type 'make' in command line for the project to be built
/// 2. type 'make remake' to rebuild your project
/// 3. type './roundrobin' to run your unit tests
//----------------------------------------------------------------------------------------------------------------------------------

int main(int argc, const char *argv[])
{
    int result = ctest_main(argc, argv);

    printf("\nRan all of the tests associated with Round Robin Scheduling\n");
    return result;
}

