#include "BlockPanel.hpp"

#include <algorithm>
#include <cmath>
#include <GL/gl.h>

namespace blastmap {
namespace ui {

namespace {
constexpr float kHeaderHeight = 24.0f;
constexpr float kContentPadding = 6.0f;
constexpr float kItemHeight = 42.0f;
constexpr float kItemSpacing = 6.0f;
constexpr float kPreviewSize = 32.0f;
constexpr float kScrollBarWidth = 6.0f;
}

BlockPanel::BlockPanel(editor::State &editor, level::LevelManager &levelManager)
    : UIPanel("Block Explorer")
    , m_editor(editor)
    , m_levelManager(levelManager)
{
}

void BlockPanel::render(int originX, int originY, int panelWidth, int panelHeight)
{
    glColor4f(0.18f, 0.18f, 0.28f, 0.92f);
    glBegin(GL_QUADS);
        glVertex2f(static_cast<float>(originX), static_cast<float>(originY));
        glVertex2f(static_cast<float>(originX + panelWidth), static_cast<float>(originY));
        glVertex2f(static_cast<float>(originX + panelWidth), static_cast<float>(originY + panelHeight));
        glVertex2f(static_cast<float>(originX), static_cast<float>(originY + panelHeight));
    glEnd();

    glColor4f(0.4f, 0.4f, 0.6f, 0.92f);
    glBegin(GL_QUADS);
        glVertex2f(static_cast<float>(originX), static_cast<float>(originY + panelHeight - kHeaderHeight));
        glVertex2f(static_cast<float>(originX + panelWidth), static_cast<float>(originY + panelHeight - kHeaderHeight));
        glVertex2f(static_cast<float>(originX + panelWidth), static_cast<float>(originY + panelHeight));
        glVertex2f(static_cast<float>(originX), static_cast<float>(originY + panelHeight));
    glEnd();

    ensureBlocks();
    drawBlockList(originX, originY, panelWidth, panelHeight);
}

void BlockPanel::onScroll(int delta)
{
    const float scrollStep = kItemHeight + kItemSpacing;
    m_scrollPosition -= static_cast<float>(delta) * (scrollStep * 0.5f);
}

void BlockPanel::ensureBlocks()
{
    if(m_editor.level == m_cachedLevel && m_editor.mode == m_cachedMode && !m_blocks.empty()) { return; }

    m_cachedLevel = m_editor.level;
    m_cachedMode = m_editor.mode;

    level::Level *level = m_levelManager.level(m_editor.level, m_editor.mode);
    if(!level)
    {
        m_blocks.clear();
        return;
    }

    m_blocks.clear();
    for(std::size_t blockIndex = 0; blockIndex < level::BlockCapacity; ++blockIndex)
    {
        BlockItem item{};
        item.id = static_cast<level::BlockID>(blockIndex);
        m_blocks.push_back(item);
    }
}

void BlockPanel::drawBlockList(int originX, int originY, int panelWidth, int panelHeight)
{
    if(m_blocks.empty()) { return; }

    const float contentX = originX + kContentPadding;
    const float contentY = originY + kContentPadding;
    const float contentHeight = panelHeight - kHeaderHeight - (kContentPadding * 2);
    if(contentHeight <= 0.0f) { return; }

    level::Level *level = m_levelManager.level(m_editor.level, m_editor.mode);
    if(!level) { return; }

    const float stride = kItemHeight + kItemSpacing;
    const float totalItemHeight =
        std::max(0.0f, (static_cast<float>(m_blocks.size()) * stride) - kItemSpacing);
    const float maxScroll = std::max(0.0f, totalItemHeight - contentHeight);
    m_scrollPosition = std::clamp(m_scrollPosition, 0.0f, maxScroll);

    int startIndex = static_cast<int>(std::floor(m_scrollPosition / stride));
    if(startIndex < 0) { startIndex = 0; }
    if(startIndex >= static_cast<int>(m_blocks.size())) { startIndex = static_cast<int>(m_blocks.size()) - 1; }
    float offset = m_scrollPosition - (startIndex * stride);
    float drawY = contentY + contentHeight - kItemHeight - offset;

    for(std::size_t idx = startIndex; idx < m_blocks.size(); ++idx)
    {
        if(drawY + kItemHeight < contentY) { break; }
        if(drawY > contentY + contentHeight) { drawY -= (kItemHeight + kItemSpacing); continue; }

        glColor4f(0.22f, 0.22f, 0.32f, 0.9f);
        glBegin(GL_QUADS);
            glVertex2f(contentX, drawY);
            glVertex2f(contentX + panelWidth - (kContentPadding * 2), drawY);
            glVertex2f(contentX + panelWidth - (kContentPadding * 2), drawY + kItemHeight);
            glVertex2f(contentX, drawY + kItemHeight);
        glEnd();

        glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(contentX, drawY);
            glVertex2f(contentX + panelWidth - (kContentPadding * 2), drawY);
            glVertex2f(contentX + panelWidth - (kContentPadding * 2), drawY + kItemHeight);
            glVertex2f(contentX, drawY + kItemHeight);
        glEnd();

        const BlockItem &item = m_blocks[idx];
        float previewX = contentX + 4.0f;
        float previewY = drawY + (kItemHeight - kPreviewSize) * 0.5f;
        drawBlockPreview(level, item, previewX, previewY, kPreviewSize);

        drawY -= (kItemHeight + kItemSpacing);
    }

    if(maxScroll > 0.0f)
    {
        const float scrollableHeight = contentHeight;
        const float totalHeight = totalItemHeight > 0.0f ? totalItemHeight : 1.0f;
        const float scrollbarHeight =
            std::max(8.0f, (scrollableHeight / totalHeight) * scrollableHeight);
        const float scrollbarX = originX + panelWidth - kContentPadding - kScrollBarWidth;
        const float scrollRatio = (maxScroll > 0.0f) ? (m_scrollPosition / maxScroll) : 0.0f;
        const float scrollbarY = contentY + (scrollableHeight - scrollbarHeight) * scrollRatio;

        glColor4f(0.1f, 0.1f, 0.1f, 0.4f);
        glBegin(GL_QUADS);
            glVertex2f(scrollbarX, contentY);
            glVertex2f(scrollbarX + kScrollBarWidth, contentY);
            glVertex2f(scrollbarX + kScrollBarWidth, contentY + scrollableHeight);
            glVertex2f(scrollbarX, contentY + scrollableHeight);
        glEnd();

        glColor4f(0.4f, 0.4f, 0.6f, 0.9f);
        glBegin(GL_QUADS);
            glVertex2f(scrollbarX, scrollbarY);
            glVertex2f(scrollbarX + kScrollBarWidth, scrollbarY);
            glVertex2f(scrollbarX + kScrollBarWidth, scrollbarY + scrollbarHeight);
            glVertex2f(scrollbarX, scrollbarY + scrollbarHeight);
        glEnd();
    }

}

void BlockPanel::drawBlockPreview(level::Level *level,
                                  const BlockItem &item,
                                  float previewX,
                                  float previewY,
                                  float size)
{
    if(!level) { return; }

    const float cellSize = size / 4.0f;
    for(int subY = 0; subY < 2; ++subY)
    {
        for(int subX = 0; subX < 2; ++subX)
        {
            level::SubBlockID subblock = (*level).Blocks[item.id][(subY * 2) + subX];
            const auto &ultras = (*level).SubBlocks[subblock];
            for(int y = 0; y < 2; ++y)
            {
                for(int x = 0; x < 2; ++x)
                {
                    level::UltraSubBlockID usblk = ultras[(y * 2) + x];
                    GLuint texid = (*level).USBTextures[usblk].texid;
                    if(texid == 0) { continue; }

                    float ultraX = previewX + ((subX * 2) + x) * cellSize;
                    float ultraY = previewY + ((subY * 2 + y)) * cellSize;
                    float invertedY = previewY + size - (ultraY - previewY) - cellSize;

                    glEnable(GL_TEXTURE_2D);
                    glBindTexture(GL_TEXTURE_2D, texid);

                    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                    glBegin(GL_QUADS);
                        glTexCoord2f(0, 0); glVertex2f(ultraX, invertedY);
                        glTexCoord2f(1, 0); glVertex2f(ultraX + cellSize, invertedY);
                        glTexCoord2f(1, 1); glVertex2f(ultraX + cellSize, invertedY + cellSize);
                        glTexCoord2f(0, 1); glVertex2f(ultraX, invertedY + cellSize);
                    glEnd();

                    glDisable(GL_TEXTURE_2D);
                }
            }
        }
    }
}

} // namespace ui
} // namespace blastmap
