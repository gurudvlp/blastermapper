#include "EditorRenderer.hpp"

namespace blastmap {
namespace view {

void EditorRenderer::Render(const editor::State &state)
{
    mapRenderer_.Render(state);
}

} // namespace view
} // namespace blastmap
