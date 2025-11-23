#ifndef BLASTMAP_LEVEL_LOADER_HPP
#define BLASTMAP_LEVEL_LOADER_HPP

#include "RomState.hpp"

#include "../level/LevelInfo.hpp"

namespace blastmap::rom {

class LevelLoader
{
    public:
        explicit LevelLoader(RomState & state);

        bool PopulateLevels();
        void LoadUSBTextures(int levelIndex, int lvlmode);

    private:
        void InitializeLevels();
        bool FindLevelPointers();
        bool LoadLevel(level::Level * level);
        void FindSpawnPoint(level::Level * level);
        void CreateUSBTextures(level::Level * level);

        RomState & state_;
};

} // namespace blastmap::rom

#endif
