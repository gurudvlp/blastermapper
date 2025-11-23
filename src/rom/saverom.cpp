#include <cstdlib>
#include <array>
#include <cstdio>
#include <cstring>

#include "Rom.hpp"
#include "RomBuilder.hpp"

namespace blastmap {
namespace rom {

bool SaveRom()
{
	auto * OutRom = builder::InitializeRom();
	if(!OutRom)
	{
		std::printf("Failed to initialize ROM buffer.\n");
		return false;
	}

	builder::CopyProgramming(OutRom);
	builder::SpawnPoints(OutRom);
	builder::Palettes(OutRom);
	builder::ScrollTables(OutRom);
	builder::LevelDataPointers(OutRom);
	builder::Maps(OutRom);

	auto * outfile = std::fopen("blasterout.nes", "w");
	if(!outfile)
	{
		std::printf("Failed to open output ROM file.\n");
		std::free(OutRom);
		return false;
	}

	std::fwrite(OutRom, SIZE_ROM_HEADER, 1, outfile);
	std::fwrite(OutRom + SIZE_ROM_HEADER, SIZE_PRG_BANK, static_cast<std::size_t>(COUNT_PRG_BANK), outfile);
	std::fclose(outfile);
	std::free(OutRom);

	std::printf("Saved rebuilt ROM\n");
	return true;
}

bool Merge(const char * sourcerom, const char * destrom, short bank)
{
	std::printf("Copying source PRG and CHR banks...\n");
	std::array<std::array<unsigned char, SIZE_PRG_BANK>, COUNT_PRG_BANK> srcPrgRom{};
	std::array<std::array<unsigned char, SIZE_CHR_BANK>, COUNT_CHR_BANK> srcChrRom{};

	for(std::size_t ebank = 0; ebank < COUNT_PRG_BANK; ++ebank)
	{
		std::memcpy(srcPrgRom[ebank].data(), PrgRom[ebank].data(), SIZE_PRG_BANK);
	}

	for(std::size_t ebank = 0; ebank < COUNT_CHR_BANK; ++ebank)
	{
		std::memcpy(srcChrRom[ebank].data(), ChrRom[ebank].data(), SIZE_CHR_BANK);
	}

	std::printf("Loading %s...\n", destrom);
	LoadRom(destrom);

	if(bank < 0 || bank >= static_cast<short>(COUNT_PRG_BANK))
	{
		std::printf("Invalid bank %d\n", bank);
		return false;
	}

	std::printf("Roms checked, starting to merge...\n");
	std::memcpy(PrgRom[static_cast<std::size_t>(bank)].data(),
	            srcPrgRom[static_cast<std::size_t>(bank)].data(),
	            SIZE_PRG_BANK);

	auto * outfile = std::fopen("blastermerge.nes", "w");
	if(!outfile)
	{
		std::printf("Failed to open merge output file.\n");
		return false;
	}

	std::fwrite(RomHeader.data(), SIZE_ROM_HEADER, 1, outfile);
	std::fwrite(PrgRom[0].data(), SIZE_PRG_BANK, COUNT_PRG_BANK, outfile);
	std::fwrite(ChrRom[0].data(), SIZE_CHR_BANK, COUNT_CHR_BANK, outfile);
	std::fclose(outfile);

	std::printf("Merged bank %d and saved as 'blastermerge.nes'\n", bank);
	return true;
}

} // namespace rom
} // namespace blastmap
