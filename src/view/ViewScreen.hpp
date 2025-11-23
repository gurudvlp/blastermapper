#ifndef HEADER_VIEW_SCREEN
#define HEADER_VIEW_SCREEN 1

#include <cstdint>

namespace blastmap {
namespace view {

class ScreenRenderer
{
public:
    void SetLevelMode(int lvl, int lvlmode);
    void SetScreen(int screen);
    void SetSelection(int x, int y);
    unsigned char SelectedBlockX() const;
    unsigned char SelectedBlockY() const;
    void Render(unsigned char darken = 0x01);

private:
    int level_ = 0;
    int mode_ = 0;
    int screen_ = 0;
    unsigned char selectedX_ = 0;
    unsigned char selectedY_ = 0;
};

} // namespace view
} // namespace blastmap

#endif
