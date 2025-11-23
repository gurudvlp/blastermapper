#ifndef HEADER_VIEW_MAP
#define HEADER_VIEW_MAP 1

#include "../editor/EditorState.hpp"

namespace blastmap {
namespace view {

class MapRenderer
{
public:
    void Render(const editor::State &state);
};

} // namespace view
} // namespace blastmap

#endif
