#ifndef HEADER_VIEW_QUADRANT
#define HEADER_VIEW_QUADRANT 1

#include <cstdint>

namespace blastmap {
namespace view {

class QuadrantRenderer
{
public:
    void SetLevelMode(int lvl, int lvlmode);
    void SetQuadrant(int quadrant);
    void SetSelection(int x, int y);
    void Render(unsigned char darken = 0x01);

private:
    int level_ = 0;
    int mode_ = 0;
    int quadrant_ = 0;
    int selectedX_ = 0;
    int selectedY_ = 0;
};

} // namespace view
} // namespace blastmap

#endif
