#ifndef BLASTMAP_LEVEL_INFO_HPP
#define BLASTMAP_LEVEL_INFO_HPP

#include "Level.hpp"

#include <array>
#include <GL/gl.h>

namespace blastmap::level {


class LevelManager
{
public:
    LevelManager();

    Level * level(int lvl, int lvlmode);
    const Level * level(int lvl, int lvlmode) const;

    unsigned char highestBlockID(int lvl, int lvlmode) const;
    unsigned char highestSubBlockID(int lvl, int lvlmode) const;
    unsigned char highestUltraSubBlockID(int lvl, int lvlmode) const;

    unsigned char blockAt(int lvl, int lvlmode, int x, int y) const;
    void setBlockAt(int lvl, int lvlmode, int x, int y, unsigned char blockid);
    void setSpawnPoint(int lvl, int lvlmode, int x, int y);
    void printSpawnPoints() const;

    short thingAt(int lvl, int lvlmode, int x, int y) const;

    unsigned char mapToBytes(unsigned char * mapdata, Level * level) const;
    unsigned char usbAttributeToByte(const USBAttributes & usbattr) const;

    unsigned char blockClipboard() const { return blockClipboard_; }
    void setBlockClipboard(unsigned char value) { blockClipboard_ = value; }

    GLuint darkenTextureID() const { return darkenTextureID_; }
    void setDarkenTextureID(GLuint id) { darkenTextureID_ = id; }

    GLuint spawnPointTextureID() const { return spawnPointTextureID_; }
    void setSpawnPointTextureID(GLuint id) { spawnPointTextureID_ = id; }

    GLuint thingSpawnTextureID() const { return thingSpawnTextureID_; }
    void setThingSpawnTextureID(GLuint id) { thingSpawnTextureID_ = id; }

    std::array<std::array<Level, LevelModes>, LevelCount> & levels() { return levels_; }
    const std::array<std::array<Level, LevelModes>, LevelCount> & levels() const { return levels_; }

private:
    bool validIndices(int lvl, int lvlmode) const;
    int findThingAt(const Level & level, unsigned char x, unsigned char y) const;

    std::array<std::array<Level, LevelModes>, LevelCount> levels_{};
    unsigned char blockClipboard_ = 0;
    GLuint darkenTextureID_ = 0;
    GLuint spawnPointTextureID_ = 0;
    GLuint thingSpawnTextureID_ = 0;
};

unsigned char MapToBytes(unsigned char * mapdata, Level * level);
unsigned char USBAttributeToByte(const USBAttributes & usbattr);

extern LevelManager gLevelManager;

} // namespace blastmap::level


#endif
