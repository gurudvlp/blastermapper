#include <stdlib.h>
#include <stdio.h>

unsigned char * build_InitializeRom();
void build_CopyProgramming(unsigned char * OutRom);
void build_InsertWatermarks(unsigned char * OutRom);
void build_SpawnPoints(unsigned char * OutRom);
void build_Palettes(unsigned char * OutRom);
void build_Maps(unsigned char * OutRom, unsigned char * highblock);
void build_ScrollTables(unsigned char * OutRom);
void build_LevelDataPointers(unsigned char * OutRom);
unsigned short build_MapData(unsigned char * mapmeta, Level * level, unsigned char highblock, SerializedMapInfo * smi);
