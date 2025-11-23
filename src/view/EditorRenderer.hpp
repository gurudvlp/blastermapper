#ifndef BLASTMAP_VIEW_EDITORRENDERER_HPP
#define BLASTMAP_VIEW_EDITORRENDERER_HPP

#include "ViewBlock.hpp"
#include "ViewMap.hpp"
#include "ViewQuadrant.hpp"
#include "ViewScreen.hpp"

#include "../editor/EditorState.hpp"

namespace blastmap {
namespace view {

class EditorRenderer
{
public:
    void Render(const editor::State &state);
    unsigned char SelectedBlockX() const;
    unsigned char SelectedBlockY() const;

private:
    MapRenderer mapRenderer_;
    QuadrantRenderer quadrantRenderer_;
    ScreenRenderer screenRenderer_;
    BlockRenderer blockRenderer_;
};

} // namespace view
} // namespace blastmap

#endif // BLASTMAP_VIEW_EDITORRENDERER_HPP
