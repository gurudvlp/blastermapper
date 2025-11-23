#include "LevelInfo.hpp"

#include <algorithm>
#include <cstring>
#include <cstdio>

namespace blastmap {
namespace level {

Level Levels[LevelCount][LevelModes];
unsigned char BlockClipboard = 0;
GLuint darkenTextureID = 0;
GLuint spawnPointTextureID = 0;
GLuint thingSpawnTextureID = 0;

namespace {

bool ValidLevelIndices(int lvl, int lvlmode)
{
    return lvl >= 0 && lvl < static_cast<int>(LevelCount)
        && lvlmode >= 0 && lvlmode < static_cast<int>(LevelModes);
}

Level * ResolveLevel(int lvl, int lvlmode)
{
    if(!ValidLevelIndices(lvl, lvlmode)) { return nullptr; }
    return &Levels[lvl][lvlmode];
}

int FindThingAt(const Level & level, unsigned char x, unsigned char y)
{
    for(std::size_t idx = 0; idx < ThingMax; ++idx)
    {
        if(level.Things[idx].x == x && level.Things[idx].y == y)
        {
            return static_cast<int>(idx);
        }
    }
    return -1;
}

} // namespace

unsigned char GetHighestBlockID(int lvl, int lvlmode)
{
    auto * level = ResolveLevel(lvl, lvlmode);
    if(!level) { return 0; }

    unsigned char highest = 0;
    for(std::size_t y = 0; y < MapHeight; ++y)
    {
        for(std::size_t x = 0; x < MapWidth; ++x)
        {
            highest = std::max(highest, level->Map[x][y]);
        }
    }
    return highest;
}

unsigned char GetHighestSubBlockID(int lvl, int lvlmode)
{
    auto * level = ResolveLevel(lvl, lvlmode);
    if(!level) { return 0; }

    unsigned char highblock = GetHighestBlockID(lvl, lvlmode);
    unsigned char highest = 0;
    for(unsigned int eb = 0; eb <= highblock; ++eb)
    {
        for(unsigned int esb = 0; esb < BlockSize; ++esb)
        {
            highest = std::max(highest, level->Blocks[eb][esb]);
        }
    }
    return highest;
}

unsigned char GetHighestUltraSubBlockID(int lvl, int lvlmode)
{
    auto * level = ResolveLevel(lvl, lvlmode);
    if(!level) { return 0; }

    unsigned char highsb = GetHighestSubBlockID(lvl, lvlmode);
    unsigned char highest = 0;
    for(unsigned int esb = 0; esb <= highsb; ++esb)
    {
        for(unsigned int eusb = 0; eusb < BlockSize; ++eusb)
        {
            highest = std::max(highest, level->SubBlocks[esb][eusb]);
        }
    }
    return highest;
}

unsigned char BlockAt(int lvl, int lvlmode, int x, int y)
{
    auto * level = ResolveLevel(lvl, lvlmode);
    return level ? level->Map[x][y] : 0;
}

void SetBlockAt(int lvl, int lvlmode, int x, int y, unsigned char blockid)
{
    auto * level = ResolveLevel(lvl, lvlmode);
    if(level) { level->Map[x][y] = blockid; }
}

void SetSpawnPoint(int lvl, int lvlmode, int x, int y)
{
    auto * level = ResolveLevel(lvl, lvlmode);
    if(!level) { return; }
    level->SpawnPoint.x = static_cast<unsigned char>(x & 0xFF);
    level->SpawnPoint.y = static_cast<unsigned char>(y & 0xFF);
}

void PrintSpawnPoints()
{
    for(std::size_t el = 0; el < LevelCount; ++el)
    {
        const Level & tank = Levels[el][0];
        const Level & overhead = Levels[el][1];

        std::printf("Level %zu Tank Spawn (%02x, %02x) (%d, %d)\n",
                    el,
                    tank.SpawnPoint.x,
                    tank.SpawnPoint.y,
                    tank.SpawnPoint.x,
                    tank.SpawnPoint.y);

        std::printf("Level %zu Overhead Spawn (%02x, %02x) (%d, %d)\n",
                    el,
                    overhead.SpawnPoint.x,
                    overhead.SpawnPoint.y,
                    overhead.SpawnPoint.x,
                    overhead.SpawnPoint.y);
    }
}

short GetThingAt(int lvl, int lvlmode, int x, int y)
{
    if(!ValidLevelIndices(lvl, lvlmode)) { return -1; }
    const Level & level = Levels[lvl][lvlmode];
    return static_cast<short>(FindThingAt(level, static_cast<unsigned char>(x & 0xFF), static_cast<unsigned char>(y & 0xFF)));
}

unsigned char MapToBytes(unsigned char * mapdata, Level * level)
{
    unsigned char highid = 0;
    for(std::size_t y = 0; y < MapHeight; ++y)
    {
        for(std::size_t x = 0; x < MapWidth; ++x)
        {
            mapdata[x + (y * MapWidth)] = level->Map[x][y];
            highid = std::max(highid, level->Map[x][y]);
        }
    }
    return highid;
}

unsigned char USBAttributeToByte(const USBAttributes & usbattr)
{
    unsigned char tbyte = 0;
    if(usbattr.gateway) { tbyte |= 0x08; }
    if(usbattr.tunnel) { tbyte |= 0x0C; }
    if(usbattr.damaging) { tbyte |= 0x10; }
    if(usbattr.softladder) { tbyte |= 0x20; }
    if(usbattr.water) { tbyte |= 0x40; }
    if(usbattr.solid) { tbyte |= 0x80; }
    if(usbattr.lava) { tbyte |= 0x50; }
    if(usbattr.solidladder) { tbyte |= 0xA0; }
    if(usbattr.destroyable) { tbyte |= 0xC0; }
    if(usbattr.doorway) { tbyte |= 0x88; }
    if(usbattr.waterladder) { tbyte |= 0x60; }
    if(usbattr.overhang) { tbyte |= 0x04; }
    if(usbattr.ice) { tbyte |= 0x14; }

    tbyte |= usbattr.subpalette;
    return tbyte;
}

} // namespace level
} // namespace blastmap
