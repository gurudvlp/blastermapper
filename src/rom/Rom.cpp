#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "Rom.hpp"
#include "LevelLoader.hpp"

#include "../level/LevelInfo.hpp"

namespace blastmap {
namespace rom {

using namespace level;

Rom::Rom()
    : loader_(state_)
    , builder_(state_)
{
}

bool Rom::load(const char * romfile)
{
    if(romfile == nullptr) { return false; }
    std::string rom(romfile);
    std::printf("Opening %s\n", rom.c_str());

    auto * romFile = std::fopen(rom.c_str(), "rb");
    if(!romFile)
    {
        std::printf("File does not exist!\n");
        std::exit(0);
    }

    auto & header = state_.Header();
    std::memset(header.data(), 0, header.size());
    std::fread(header.data(), header.size(), 1, romFile);

    if(!(header[0] == 0x4E && header[1] == 0x45 && header[2] == 0x53 && header[3] == 0x1A))
    {
        std::fclose(romFile);
        std::printf("Invalid ROM file.\n");
        std::exit(0);
    }

    unsigned short prgsize = header[4];
    unsigned short chrsize = header[5];

    if(prgsize != COUNT_PRG_BANK)
    {
        std::fclose(romFile);
        std::printf("This ROM does not have %zu PRGROM banks.\n", COUNT_PRG_BANK);
        std::exit(0);
    }

    if(chrsize != COUNT_CHR_BANK)
    {
        std::fclose(romFile);
        std::printf("This ROM does not have %zu CHRROM banks.\n", COUNT_CHR_BANK);
        std::exit(0);
    }

    auto & prgRom = state_.PrgRom();
    auto & chrRom = state_.ChrRom();

    for(int ebank = 0; ebank < static_cast<int>(COUNT_PRG_BANK); ++ebank)
    {
        std::fread(prgRom[ebank].data(), SIZE_PRG_BANK, 1, romFile);
    }

    for(int ebank = 0; ebank < static_cast<int>(COUNT_CHR_BANK); ++ebank)
    {
        std::fread(chrRom[ebank].data(), SIZE_CHR_BANK, 1, romFile);
    }

    std::fclose(romFile);

    std::printf("ROM successfully loaded to RAM.\n");

    if(!loader_.PopulateLevels())
    {
        std::printf("Could not determine the pointers to the level data.\n");
        std::exit(0);
    }

    std::printf("Levels successfully loaded.\n");

    return true;
}

bool Rom::save(const char * outfile)
{
    auto * OutRom = builder_.InitializeRom();
    if(!OutRom)
    {
        std::printf("Failed to initialize ROM buffer.\n");
        return false;
    }

    builder_.CopyProgramming(OutRom);
    builder_.SpawnPoints(OutRom);
    builder_.Palettes(OutRom);
    builder_.ScrollTables(OutRom);
    builder_.LevelDataPointers(OutRom);
    builder_.Maps(OutRom);

    auto * file = std::fopen(outfile, "w");
    if(!file)
    {
        std::printf("Failed to open output ROM file.\n");
        std::free(OutRom);
        return false;
    }

    std::fwrite(OutRom, SIZE_ROM_HEADER, 1, file);
    std::fwrite(OutRom + SIZE_ROM_HEADER, SIZE_PRG_BANK, COUNT_PRG_BANK, file);
    std::fclose(file);
    std::free(OutRom);

    std::printf("Saved rebuilt ROM\n");
    return true;
}

bool Rom::merge(const char * sourcerom, const char * destrom, short bank)
{
    std::printf("Copying source PRG and CHR banks...\n");
    std::array<std::array<unsigned char, SIZE_PRG_BANK>, COUNT_PRG_BANK> srcPrgRom{};
    std::array<std::array<unsigned char, SIZE_CHR_BANK>, COUNT_CHR_BANK> srcChrRom{};

    auto & prgRom = state_.PrgRom();
    auto & chrRom = state_.ChrRom();

    for(std::size_t ebank = 0; ebank < COUNT_PRG_BANK; ++ebank)
    {
        std::memcpy(srcPrgRom[ebank].data(), prgRom[ebank].data(), SIZE_PRG_BANK);
    }

    for(std::size_t ebank = 0; ebank < COUNT_CHR_BANK; ++ebank)
    {
        std::memcpy(srcChrRom[ebank].data(), chrRom[ebank].data(), SIZE_CHR_BANK);
    }

    std::printf("Loading %s...\n", destrom);
    if(!load(destrom))
    {
        std::printf("Failed to load destination ROM.\n");
        return false;
    }

    if(bank < 0 || bank >= static_cast<short>(COUNT_PRG_BANK))
    {
        std::printf("Invalid bank %d\n", bank);
        return false;
    }

    std::printf("Roms checked, starting to merge...\n");
    auto & mergedPrgRom = state_.PrgRom();
    std::memcpy(mergedPrgRom[static_cast<std::size_t>(bank)].data(),
                srcPrgRom[static_cast<std::size_t>(bank)].data(),
                SIZE_PRG_BANK);

    auto * outfile = std::fopen("blastermerge.nes", "w");
    if(!outfile)
    {
        std::printf("Failed to open merge output file.\n");
        return false;
    }

    auto & header = state_.Header();
    std::fwrite(header.data(), SIZE_ROM_HEADER, 1, outfile);
    std::fwrite(mergedPrgRom[0].data(), SIZE_PRG_BANK, COUNT_PRG_BANK, outfile);
    auto & mergedChrRom = state_.ChrRom();
    std::fwrite(mergedChrRom[0].data(), SIZE_CHR_BANK, COUNT_CHR_BANK, outfile);
    std::fclose(outfile);

    std::printf("Merged bank %d and saved as 'blastermerge.nes'\n", bank);
    return true;
}

void Rom::loadUSBTextures(int level, int lvlmode)
{
    loader_.LoadUSBTextures(level, lvlmode);
}

void Rom::printThings(int level, unsigned char leveltype) const
{
    if(level < 0 || level > 7) { return; }
    if(leveltype != 0x00 && leveltype != 0x01) { return; }

    const Level * lvl = level::gLevelManager.level(level, leveltype);

    std::printf("Thing list for level %d (mode %x)\n", level, leveltype);

    for(int x = 0; x < 512; x++)
    {
        if(lvl->Things[x].thingtype == 0xFF) { break; }

        std::printf("Thing %d: Type: %02x\t(%02x, %02x)\n",
                    x,
                    lvl->Things[x].thingtype,
                    lvl->Things[x].x,
                    lvl->Things[x].y);
    }
}

} // namespace rom
} // namespace blastmap
