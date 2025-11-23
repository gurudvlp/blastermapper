#ifndef HEADER_LEVEL_LEVELINFO
#define HEADER_LEVEL_LEVELINFO 1

#include <stdlib.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#ifdef __cplusplus
extern "C" {
#endif

unsigned char GetHighestBlockID(int lvl, int lvlmode);
unsigned char GetHighestSubBlockID(int lvl, int lvlmode);
unsigned char GetHighestUltraSubBlockID(int lvl, int lvlmode);
unsigned char BlockAt(int lvl, int lvlmode, int x, int y);
void SetBlockAt(int lvl, int lvlmode, int x, int y, unsigned char blockid);
extern unsigned char BlockClipboard;
void SetSpawnPoint(int lvl, int lvlmode, int x, int y);
void PrintSpawnPoints();
short GetThingAt(int lvl, int lvlmode, int x, int y);
unsigned char level_MapToBytes(unsigned char * mapdata, Level * level);
unsigned char level_USBAttributeToByte(USBAttributes * usbattr);

extern GLuint darkenTextureID;
extern GLuint spawnPointTextureID;
extern GLuint thingSpawnTextureID;

#ifdef __cplusplus
}
#endif

#endif
