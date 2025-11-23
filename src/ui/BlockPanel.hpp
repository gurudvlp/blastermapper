#ifndef BLASTMAP_UI_BLOCKPANEL_HPP
#define BLASTMAP_UI_BLOCKPANEL_HPP

#include "UIPanel.hpp"

#include "../editor/EditorState.hpp"
#include "../level/LevelInfo.hpp"

#include <vector>

namespace blastmap {
namespace ui {

class BlockPanel : public UIPanel
{
public:
    BlockPanel(editor::State &editor, level::LevelManager &levelManager);

    void render(int originX, int originY, int panelWidth, int panelHeight) override;
    void onScroll(int delta) override;

private:
    struct BlockItem
    {
        level::BlockID id = 0;
    };

    void ensureBlocks();
    void drawBlockList(int originX, int originY, int panelWidth, int panelHeight);
    void drawBlockPreview(level::Level *level, const BlockItem &item, float previewX, float previewY, float size);

    editor::State &m_editor;
    level::LevelManager &m_levelManager;
    int m_cachedLevel = -1;
    int m_cachedMode = -1;
    std::vector<BlockItem> m_blocks;
    float m_scrollPosition = 0.0f;
};

} // namespace ui
} // namespace blastmap

#endif // BLASTMAP_UI_BLOCKPANEL_HPP
