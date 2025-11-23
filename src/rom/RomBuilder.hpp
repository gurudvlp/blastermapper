#ifndef BLASTMAP_ROM_BUILDER_HPP
#define BLASTMAP_ROM_BUILDER_HPP

#include "RomState.hpp"

#include "../level/LevelInfo.hpp"

namespace blastmap::rom::builder {

class RomBuilder
{
    public:
        explicit RomBuilder(RomState & state);

        unsigned char * InitializeRom();
        void CopyProgramming(unsigned char * outRom);
        void InsertWatermarks(unsigned char * outRom);
        void SpawnPoints(unsigned char * outRom);
        void Palettes(unsigned char * outRom);
        void Maps(unsigned char * outRom);
        void ScrollTables(unsigned char * outRom);
        void LevelDataPointers(unsigned char * outRom);

    private:
        void SubPalettes(level::Level * level, unsigned char * spbytes);
        unsigned short MapData(unsigned char * mapmeta,
                            level::Level * level,
                            unsigned char highblock,
                            SerializedMapInfo * smi);
        void MapPointers(level::Level * level,
                        unsigned short startloc,
                        SerializedMapInfo * smi,
                        unsigned char * outbuf);
        void OutRomAddressToBytes(unsigned short addr, unsigned char * bytes);

        RomState & state_;
};

} // namespace blastmap::rom::builder


#endif
