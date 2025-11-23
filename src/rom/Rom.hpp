#ifndef BLASTMAP_ROM_HPP
#define BLASTMAP_ROM_HPP

#include "RomState.hpp"
#include "LevelLoader.hpp"
#include "RomBuilder.hpp"

namespace blastmap::rom {

class Rom
{
    public:
        Rom();

        bool load(const char * romfile);
        bool save(const char * outfile = "blasterout.nes");
        bool merge(const char * sourcerom, const char * destrom, short bank);
        void loadUSBTextures(int level, int lvlmode);
        void printThings(int level, unsigned char leveltype) const;

    private:
        RomState state_;
        LevelLoader loader_;
        builder::RomBuilder builder_;
};

} // namespace blastmap::rom

#endif
