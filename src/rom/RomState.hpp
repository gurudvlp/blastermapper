#ifndef BLASTMAP_ROM_STATE_HPP
#define BLASTMAP_ROM_STATE_HPP

#include <array>
#include <cstddef>
#include <cstdint>

#include "../level/Level.hpp"
#include "../level/LevelInfo.hpp"

namespace blastmap::rom {

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

struct SerializedMapInfo
{
    unsigned short BlockSize;
    unsigned short SubBlockSize;
    unsigned short USBSize;
    unsigned short USBAttrSize;
};

class RomState
{
    public:
        using PrgBank = std::array<unsigned char, SIZE_PRG_BANK>;
        using ChrBank = std::array<unsigned char, SIZE_CHR_BANK>;

        std::array<PrgBank, COUNT_PRG_BANK> & PrgRom() noexcept { return prgRom_; }
        const std::array<PrgBank, COUNT_PRG_BANK> & PrgRom() const noexcept { return prgRom_; }

        std::array<ChrBank, COUNT_CHR_BANK> & ChrRom() noexcept { return chrRom_; }
        const std::array<ChrBank, COUNT_CHR_BANK> & ChrRom() const noexcept { return chrRom_; }

        std::array<unsigned char, SIZE_ROM_HEADER> & Header() noexcept { return romHeader_; }
        const std::array<unsigned char, SIZE_ROM_HEADER> & Header() const noexcept { return romHeader_; }

    private:
        std::array<PrgBank, COUNT_PRG_BANK> prgRom_{};
        std::array<ChrBank, COUNT_CHR_BANK> chrRom_{};
        std::array<unsigned char, SIZE_ROM_HEADER> romHeader_{};
};

} // namespace blastmap::rom

#endif
