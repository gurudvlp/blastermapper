#ifndef BLASTMAP_VIEW_EDITORRENDERER_HPP
#define BLASTMAP_VIEW_EDITORRENDERER_HPP

#include "ViewMap.hpp"

#include "../editor/EditorState.hpp"

namespace blastmap {
namespace view {

class EditorRenderer
{
public:
    void Render(const editor::State &state);
private:
    MapRenderer mapRenderer_;
};

} // namespace view
} // namespace blastmap

#endif // BLASTMAP_VIEW_EDITORRENDERER_HPP
