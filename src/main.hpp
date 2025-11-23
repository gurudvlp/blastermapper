#ifndef HEADER_MAIN
#define HEADER_MAIN

#include <stdlib.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

#define WindowWidth 768
#define WindowHeight 768

#ifdef __cplusplus
extern "C" {
#endif

extern bool SAVEROM_SHOW_LEVEL_POINTERS;
bool IsCmdOptionSet(int argc, char ** argv, const char option[]);

#ifdef __cplusplus
}
#endif

#endif
