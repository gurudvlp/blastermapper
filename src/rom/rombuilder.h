#include <stdlib.h>
#include <stdio.h>

unsigned char * build_InitializeRom();
void build_CopyProgramming(unsigned char * OutRom);
void build_InsertWatermarks(unsigned char * OutRom);
void build_SpawnPoints(unsigned char * OutRom);
void build_Palettes(unsigned char * OutRom);
void build_Maps(unsigned char * OutRom);
void build_ScrollTables(unsigned char * OutRom);
void build_LevelDataPointers(unsigned char * OutRom);
