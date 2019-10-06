#include <stdlib.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

unsigned char GetHighestBlockID(int lvl, int lvlmode);
unsigned char GetHighestSubBlockID(int lvl, int lvlmode);
unsigned char GetHighestUltraSubBlockID(int lvl, int lvlmode);
unsigned char BlockAt(int lvl, int lvlmode, int x, int y);
void SetBlockAt(int lvl, int lvlmode, int x, int y, unsigned char blockid);
unsigned char BlockClipboard;
void SetSpawnPoint(int lvl, int lvlmode, int x, int y);
void PrintSpawnPoints();
short GetThingAt(int lvl, int lvlmode, int x, int y);
unsigned char level_MapToBytes(unsigned char * mapdata, Level * level);

GLuint darkenTextureID;
GLuint spawnPointTextureID;
GLuint thingSpawnTextureID;
