#ifndef HEADER_MAIN
#define HEADER_MAIN

//#include <stdbool.h>
#include <stdlib.h>

typedef int bool;
#define true 1
#define false 0

#define WindowWidth 768
#define WindowHeight 768

bool SAVEROM_SHOW_LEVEL_POINTERS;
bool IsCmdOptionSet(int argc, char ** argv, char option[]);

#endif