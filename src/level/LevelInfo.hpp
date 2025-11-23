#ifndef BLASTMAP_LEVEL_INFO_HPP
#define BLASTMAP_LEVEL_INFO_HPP

#include "Level.hpp"

#include <GL/gl.h>

namespace blastmap {
namespace level {

extern unsigned char BlockClipboard;
extern GLuint darkenTextureID;
extern GLuint spawnPointTextureID;
extern GLuint thingSpawnTextureID;

unsigned char GetHighestBlockID(int lvl, int lvlmode);
unsigned char GetHighestSubBlockID(int lvl, int lvlmode);
unsigned char GetHighestUltraSubBlockID(int lvl, int lvlmode);
unsigned char BlockAt(int lvl, int lvlmode, int x, int y);
void SetBlockAt(int lvl, int lvlmode, int x, int y, unsigned char blockid);
void SetSpawnPoint(int lvl, int lvlmode, int x, int y);
void PrintSpawnPoints();
short GetThingAt(int lvl, int lvlmode, int x, int y);
unsigned char MapToBytes(unsigned char * mapdata, Level * level);
unsigned char USBAttributeToByte(const USBAttributes & usbattr);

} // namespace level
} // namespace blastmap

#endif
