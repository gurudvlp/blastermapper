#ifndef HEADER_VIEW_BLOCK
#define HEADER_VIEW_BLOCK 1

#include <cstdint>

namespace blastmap {
namespace view {

class BlockRenderer
{
public:
    void SetLevelMode(int lvl, int lvlmode);
    void SetBlock(int block);
    void Render();

private:
    int level_ = 0;
    int mode_ = 0;
    int block_ = 0;
};

} // namespace view
} // namespace blastmap

#endif
