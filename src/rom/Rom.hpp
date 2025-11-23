#ifndef BLASTMAP_ROM_HPP
#define BLASTMAP_ROM_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>

#include "../main.hpp"
#include "../level/Level.hpp"
#include "../level/LevelInfo.hpp"

namespace blastmap {
namespace rom {

inline constexpr std::size_t SIZE_PRG_BANK = 0x4000;
inline constexpr std::size_t SIZE_CHR_BANK = 0x2000;
inline constexpr std::size_t SIZE_ROM_HEADER = 16;
inline constexpr std::size_t SIZE_PALETTE = 16;
inline constexpr std::size_t SIZE_LEVELDATAPOINTERS = 12;
inline constexpr std::size_t SIZE_MAP = 0x0400;
inline constexpr std::size_t SIZE_SCROLLTABLE = 16;

inline constexpr std::size_t COUNT_PRG_BANK = 8;
inline constexpr std::size_t COUNT_CHR_BANK = 16;

inline constexpr std::size_t OFFSET_SPAWNS_TANK = 0x1CA4B;
inline constexpr std::size_t OFFSET_SPAWNS_AFTERBOSS = 0x1C5B2;

inline constexpr std::size_t OFFSET_BANK_0 = 0x0000;
inline constexpr std::size_t OFFSET_BANK_1 = 0x4000;
inline constexpr std::size_t OFFSET_BANK_2 = 0x8000;
inline constexpr std::size_t OFFSET_BANK_3 = 0xC000;

inline constexpr std::size_t OFFSET_PALETTE = 0x0092 + SIZE_ROM_HEADER;
inline constexpr std::size_t OFFSET_SCROLLTABLE = 0x00F2 + SIZE_ROM_HEADER;
inline constexpr std::size_t OFFSET_MAP = 0x0152 + SIZE_ROM_HEADER;
inline constexpr std::size_t OFFSET_MAPDATA = 0x1660 + SIZE_ROM_HEADER;
inline constexpr std::size_t OFFSET_LEVELDATAPOINTERS = 0x0020 + SIZE_ROM_HEADER;

extern FILE * RomFile;
extern std::array<std::array<unsigned char, SIZE_PRG_BANK>, COUNT_PRG_BANK> PrgRom;
extern std::array<std::array<unsigned char, SIZE_CHR_BANK>, COUNT_CHR_BANK> ChrRom;
extern std::array<unsigned char, SIZE_ROM_HEADER> RomHeader;

struct SerializedMapInfo
{
    unsigned short BlockSize;
    unsigned short SubBlockSize;
    unsigned short USBSize;
    unsigned short USBAttrSize;
};

bool LoadRom(const char * romfile);
void LoadUSBTextures(int level, int lvlmode);
bool SaveRom();
bool Merge(const char * sourcerom, const char * destrom, short bank);
void PrintThings(int level, unsigned char leveltype);
void PrintLevelPointer(const char * text, const unsigned char * bytes);

} // namespace rom
} // namespace blastmap

#endif
