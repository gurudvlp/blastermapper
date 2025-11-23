#ifndef BLASTMAP_ROM_BUILDER_HPP
#define BLASTMAP_ROM_BUILDER_HPP

#include "Rom.hpp"

namespace blastmap {
namespace rom {
namespace builder {

unsigned char * InitializeRom();
void CopyProgramming(unsigned char * outRom);
void InsertWatermarks(unsigned char * outRom);
void SpawnPoints(unsigned char * outRom);
void Palettes(unsigned char * outRom);
void Maps(unsigned char * outRom);
void ScrollTables(unsigned char * outRom);
void LevelDataPointers(unsigned char * outRom);
unsigned short MapData(unsigned char * mapmeta,
                       level::Level * level,
                       unsigned char highblock,
                       SerializedMapInfo * smi);
void MapPointers(level::Level * level,
                 unsigned short startloc,
                 SerializedMapInfo * smi,
                 unsigned char * outbuf);
void SubPalettes(level::Level * level, unsigned char * spbytes);
void OutRomAddressToBytes(unsigned short addr, unsigned char * bytes);

} // namespace builder
} // namespace rom
} // namespace blastmap

#endif
