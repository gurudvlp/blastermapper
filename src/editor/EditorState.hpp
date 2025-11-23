#ifndef BLASTMAP_EDITOR_EDITORSTATE_HPP
#define BLASTMAP_EDITOR_EDITORSTATE_HPP

#include <cstdint>

namespace blastmap {
namespace editor {

struct State
{
    static constexpr float MapDimension = 128.0f;
    static constexpr float MapHalf = MapDimension / 2.0f;

    int level = 0;
    unsigned char mode = 0x00;
    float viewCenterX = MapHalf;
    float viewCenterY = MapHalf;
    float viewZoom = 1.0f;
};

} // namespace editor
} // namespace blastmap

#endif // BLASTMAP_EDITOR_EDITORSTATE_HPP
