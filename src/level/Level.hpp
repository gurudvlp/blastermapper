#ifndef BLASTMAP_LEVEL_HPP
#define BLASTMAP_LEVEL_HPP

#include <array>
#include <cstddef>
#include <cstdint>

#include <GL/gl.h>

namespace blastmap {
namespace level {

using LevelTypeValue = std::uint8_t;
enum class LevelType : LevelTypeValue
{
    Tank = 0x00,
    Overhead = 0x01,
};

using BlockID = std::uint8_t;
using SubBlockID = std::uint8_t;
using UltraSubBlockID = std::uint8_t;
using ChrID = std::uint8_t;

inline constexpr std::size_t LevelCount = 8;
inline constexpr std::size_t LevelModes = 2;
inline constexpr std::size_t MapWidth = 32;
inline constexpr std::size_t MapHeight = 32;
inline constexpr std::size_t BlockSize = 4;
inline constexpr std::size_t BlockCapacity = 256;
inline constexpr std::size_t ThingMax = 512;
inline constexpr std::size_t USBTextureSize = 1024;

struct ThingListPointers
{
    std::uint16_t listpointer;
    std::uint16_t typelistpointer;
    std::uint16_t xlistpointer;
    std::uint16_t ylistpointer;
};

struct LevelDataPointers
{
    std::uint16_t palette;
    std::uint16_t usbattribute;
    std::uint16_t usbtable;
    std::uint16_t sbtable;
    std::uint16_t blocktable;
    std::uint16_t map;
    std::uint16_t scrolltable;
    ThingListPointers thinglist;
};

struct LevelPalette
{
    std::uint8_t color0 = 0;
    std::uint8_t color1 = 0;
    std::uint8_t color2 = 0;
    std::uint8_t color3 = 0;
};

struct USBAttributes
{
    std::uint8_t subpalette = 0;
    bool gateway = false;
    bool tunnel = false;
    bool damaging = false;
    bool softladder = false;
    bool water = false;
    bool solid = false;
    bool lava = false;
    bool solidladder = false;
    bool destroyable = false;
    bool doorway = false;
    bool waterladder = false;
    bool overhang = false;
    bool ice = false;
};

struct Thing
{
    std::uint8_t thingtype = 0xFF;
    std::uint8_t x = 0;
    std::uint8_t y = 0;
    GLuint texid = 0;
};

struct SpawnCoords
{
    std::uint8_t x = 0;
    std::uint8_t y = 0;
};

struct USBTexture
{
    std::array<GLubyte, USBTextureSize> data{};
    GLuint texid = 0;
};

struct Level
{
    LevelType leveltype = LevelType::Tank;
    std::uint16_t levelid = 0;
    std::uint16_t romlevelpointer = 0;
    LevelDataPointers datapointers{};
    std::array<std::array<BlockID, MapHeight>, MapWidth> Map{};
    std::array<std::array<SubBlockID, BlockSize>, BlockCapacity> Blocks{};
    std::array<std::array<UltraSubBlockID, BlockSize>, BlockCapacity> SubBlocks{};
    std::array<std::array<ChrID, BlockSize>, BlockCapacity> UltraSubBlocks{};
    std::array<USBAttributes, BlockCapacity> USBAttributeTable{};
    std::array<USBTexture, BlockCapacity> USBTextures{};
    std::array<LevelPalette, 4> SubPalettes{};
    std::array<std::uint8_t, 16> ScrollTable{};
    std::array<Thing, ThingMax> Things{};
    SpawnCoords SpawnPoint{};
};

extern Level Levels[LevelCount][LevelModes];

} // namespace level
} // namespace blastmap

#endif
