#ifndef BLASTMAP_EDITOR_EDITORSTATE_HPP
#define BLASTMAP_EDITOR_EDITORSTATE_HPP

#include <array>

namespace blastmap {
namespace editor {

enum class ZoomMode : int
{
    Map = 0,
    Quadrant = 1,
    Screen = 2,
    Block = 3,
};

struct State
{
    ZoomMode zoom = ZoomMode::Map;
    int level = 0;
    unsigned char mode = 0x00;
    unsigned char quadrant = 0x00;
    unsigned short x = 0;
    unsigned short y = 0;
    unsigned short xSelect = 0;
    unsigned short ySelect = 0;
    unsigned short thing = 0;
    std::array<std::array<unsigned short, 4>, 4> coords = {};

    void saveCoords()
    {
        auto &slot = coords[static_cast<int>(zoom)];
        slot[0] = x;
        slot[1] = y;
        slot[2] = xSelect;
        slot[3] = ySelect;
    }

    void restoreCoords(ZoomMode target)
    {
        auto &slot = coords[static_cast<int>(target)];
        x = slot[0];
        y = slot[1];
        xSelect = slot[2];
        ySelect = slot[3];
    }

    void zoomIn()
    {
        if(zoom == ZoomMode::Block) { return; }
        saveCoords();
        zoom = static_cast<ZoomMode>(static_cast<int>(zoom) + 1);

        if(zoom == ZoomMode::Quadrant)
        {
            x = xSelect;
            y = ySelect;
        }
        else if(zoom == ZoomMode::Screen)
        {
            x = (x == 1) ? (4 + xSelect) : xSelect;
            y = (y == 1) ? (4 + ySelect) : ySelect;
        }

        xSelect = 0;
        ySelect = 0;
    }

    void zoomOut()
    {
        if(zoom == ZoomMode::Map) { return; }
        zoom = static_cast<ZoomMode>(static_cast<int>(zoom) - 1);
        restoreCoords(zoom);
    }

    int selectionMax() const
    {
        switch(zoom)
        {
            case ZoomMode::Map:
            case ZoomMode::Block:
                return 1;
            case ZoomMode::Quadrant:
            case ZoomMode::Screen:
                return 3;
        }
        return 1;
    }

    void moveLeft()
    {
        if(xSelect == 0) { xSelect = selectionMax(); }
        else { --xSelect; }
    }

    void moveRight()
    {
        if(xSelect == selectionMax()) { xSelect = 0; }
        else { ++xSelect; }
    }

    void moveUp()
    {
        if(ySelect == 0) { ySelect = selectionMax(); }
        else { --ySelect; }
    }

    void moveDown()
    {
        if(ySelect == selectionMax()) { ySelect = 0; }
        else { ++ySelect; }
    }
};

} // namespace editor
} // namespace blastmap

#endif // BLASTMAP_EDITOR_EDITORSTATE_HPP
