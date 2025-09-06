#ifndef HEADER_ROM_LOADROM
#define HEADER_ROM_LOADROM 1

//#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


#include "../main.h"
#include "../level/leveltypes.h"

bool LoadRom(char * romfile);

void InitializeLevels();
bool FindLevelPointers();
bool LoadLevel(Level * level);
void FindSpawnPoint(Level * level);


void CreateUSBTextures(Level * level);
void LoadUSBTextures(int level, int lvlmode);

void PrintThings(int level, unsigned char leveltype);
void PrintLevelPointer(char * text, unsigned char * bytes);

FILE * RomFile;

unsigned char RomHeader[16];
unsigned char PrgRom[8][0x4000];
unsigned char ChrRom[16][0x2000];

#endif