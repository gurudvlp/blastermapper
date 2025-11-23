#include "LevelInfo.hpp"

#include <algorithm>
#include <cstdio>

namespace blastmap::level {

LevelManager gLevelManager;

LevelManager::LevelManager() = default;

bool LevelManager::validIndices(int lvl, int lvlmode) const
{
    return lvl >= 0 && lvl < static_cast<int>(LevelCount)
        && lvlmode >= 0 && lvlmode < static_cast<int>(LevelModes);
}

Level * LevelManager::level(int lvl, int lvlmode)
{
    if(!validIndices(lvl, lvlmode)) { return nullptr; }
    return &levels_[lvl][lvlmode];
}

const Level * LevelManager::level(int lvl, int lvlmode) const
{
    if(!validIndices(lvl, lvlmode)) { return nullptr; }
    return &levels_[lvl][lvlmode];
}

unsigned char LevelManager::highestBlockID(int lvl, int lvlmode) const
{
    const Level * lvlptr = level(lvl, lvlmode);
    if(!lvlptr) { return 0; }

    unsigned char highest = 0;
    for(std::size_t y = 0; y < MapHeight; ++y)
    {
        for(std::size_t x = 0; x < MapWidth; ++x)
        {
            highest = std::max(highest, lvlptr->Map[x][y]);
        }
    }
    return highest;
}

unsigned char LevelManager::highestSubBlockID(int lvl, int lvlmode) const
{
    auto highblock = highestBlockID(lvl, lvlmode);
    auto * levelPtr = level(lvl, lvlmode);
    if(!levelPtr) { return 0; }

    unsigned char highest = 0;
    for(unsigned int eb = 0; eb <= highblock; ++eb)
    {
        for(unsigned int esb = 0; esb < BlockSize; ++esb)
        {
            highest = std::max(highest, levelPtr->Blocks[eb][esb]);
        }
    }
    return highest;
}

unsigned char LevelManager::highestUltraSubBlockID(int lvl, int lvlmode) const
{
    auto highsb = highestSubBlockID(lvl, lvlmode);
    auto * levelPtr = level(lvl, lvlmode);
    if(!levelPtr) { return 0; }

    unsigned char highest = 0;
    for(unsigned int esb = 0; esb <= highsb; ++esb)
    {
        for(unsigned int eusb = 0; eusb < BlockSize; ++eusb)
        {
            highest = std::max(highest, levelPtr->SubBlocks[esb][eusb]);
        }
    }
    return highest;
}

unsigned char LevelManager::blockAt(int lvl, int lvlmode, int x, int y) const
{
    auto * levelPtr = level(lvl, lvlmode);
    return levelPtr ? levelPtr->Map[x][y] : 0;
}

void LevelManager::setBlockAt(int lvl, int lvlmode, int x, int y, unsigned char blockid)
{
    auto * levelPtr = level(lvl, lvlmode);
    if(levelPtr) { levelPtr->Map[x][y] = blockid; }
}

void LevelManager::setSpawnPoint(int lvl, int lvlmode, int x, int y)
{
    auto * levelPtr = level(lvl, lvlmode);
    if(!levelPtr) { return; }
    levelPtr->SpawnPoint.x = static_cast<unsigned char>(x & 0xFF);
    levelPtr->SpawnPoint.y = static_cast<unsigned char>(y & 0xFF);
}

void LevelManager::printSpawnPoints() const
{
    for(std::size_t el = 0; el < LevelCount; ++el)
    {
        const Level & tank = levels_[el][0];
        const Level & overhead = levels_[el][1];

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

int LevelManager::findThingAt(const Level & level, unsigned char x, unsigned char y) const
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

short LevelManager::thingAt(int lvl, int lvlmode, int x, int y) const
{
    auto * levelPtr = level(lvl, lvlmode);
    if(!levelPtr) { return -1; }
    return static_cast<short>(findThingAt(*levelPtr,
                                          static_cast<unsigned char>(x & 0xFF),
                                          static_cast<unsigned char>(y & 0xFF)));
}

unsigned char LevelManager::mapToBytes(unsigned char * mapdata, Level * level) const
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

unsigned char LevelManager::usbAttributeToByte(const USBAttributes & usbattr) const
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

unsigned char MapToBytes(unsigned char * mapdata, Level * level)
{
    return gLevelManager.mapToBytes(mapdata, level);
}

unsigned char USBAttributeToByte(const USBAttributes & usbattr)
{
    return gLevelManager.usbAttributeToByte(usbattr);
}

} // namespace blastmap::level

