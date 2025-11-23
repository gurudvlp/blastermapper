#include "EditorRenderer.hpp"

namespace blastmap {
namespace view {

void EditorRenderer::Render(const editor::State &state)
{
    switch(state.zoom)
    {
        case editor::ZoomMode::Map:
            mapRenderer_.SetLevelMode(state.level, state.mode);
            mapRenderer_.SetSelection(state.xSelect, state.ySelect);
            mapRenderer_.Render(0x01);
            break;
        case editor::ZoomMode::Quadrant:
            quadrantRenderer_.SetLevelMode(state.level, state.mode);
            quadrantRenderer_.SetQuadrant(state.quadrant);
            quadrantRenderer_.SetSelection(state.xSelect, state.ySelect);
            quadrantRenderer_.Render(0x01);
            break;
        case editor::ZoomMode::Screen:
            screenRenderer_.SetLevelMode(state.level, state.mode);
            screenRenderer_.SetScreen((state.y * 8) + state.x);
            screenRenderer_.SetSelection(state.xSelect, state.ySelect);
            screenRenderer_.Render(0x01);
            break;
        case editor::ZoomMode::Block:
            blockRenderer_.SetLevelMode(state.level, state.mode);
            blockRenderer_.SetBlock((state.y * 32) + state.x);
            blockRenderer_.Render();
            break;
    }
}

unsigned char EditorRenderer::SelectedBlockX() const
{
    return screenRenderer_.SelectedBlockX();
}

unsigned char EditorRenderer::SelectedBlockY() const
{
    return screenRenderer_.SelectedBlockY();
}

} // namespace view
} // namespace blastmap
