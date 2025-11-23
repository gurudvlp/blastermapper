#include "UIService.hpp"

#include <algorithm>
#include <GL/gl.h>

namespace blastmap {
namespace ui {

namespace {
constexpr int kHotbarIconPadding = 8;
}

UIService::UIService(event::EventManager &eventManager)
    : m_eventManager(eventManager)
{
    auto addPanel = [this](const std::string &label, std::unique_ptr<UIPanel> panel) {
        if(!panel) { return; }
        hotbar_.push_back({label, panel.get()});
        panels_.push_back(std::move(panel));
    };

    addPanel("Blocks", std::make_unique<DummyPanel>("Block Explorer"));
    addPanel("Palette", std::make_unique<DummyPanel>("Palette"));
    addPanel("Things", std::make_unique<DummyPanel>("Thing Explorer"));

    m_eventManager.subscribe<event::MouseButtonEvent>(
        [this](const event::MouseButtonEvent &payload) { handleMouseButton(payload); });
}

void UIService::render()
{
    //return;
    int width = 0;
    int height = 0;
    if(!getWindowSize(width, height) || width == 0 || height == 0) { return; }

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, static_cast<double>(width), 0.0, static_cast<double>(height), -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    GLboolean depthEnabled = glIsEnabled(GL_DEPTH_TEST);
    GLboolean textureEnabled = glIsEnabled(GL_TEXTURE_2D);
    GLboolean blendEnabled = glIsEnabled(GL_BLEND);

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glDisable(GL_TEXTURE_2D);
    if(!blendEnabled) { glEnable(GL_BLEND); }

    drawHotbar(width, height);
    drawPanels(width, height);

    glDepthMask(GL_TRUE);
    if(depthEnabled) { glEnable(GL_DEPTH_TEST); }
    else { glDisable(GL_DEPTH_TEST); }
    if(textureEnabled) { glEnable(GL_TEXTURE_2D); }
    else { glDisable(GL_TEXTURE_2D); }
    if(!blendEnabled) { glDisable(GL_BLEND); }

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void UIService::drawHotbar(int width, int height)
{
    if(hotbar_.empty()) { return; }

    glColor4f(0.06f, 0.06f, 0.08f, 0.9f);
    glBegin(GL_QUADS);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(static_cast<float>(width), 0.0f);
        glVertex2f(static_cast<float>(width), static_cast<float>(m_hotbarHeight));
        glVertex2f(0.0f, static_cast<float>(m_hotbarHeight));
    glEnd();

    float slotWidth = static_cast<float>(width) / static_cast<float>(hotbar_.size());
    float iconSize = std::max(8.0f, std::min(slotWidth, static_cast<float>(m_hotbarHeight)) - (kHotbarIconPadding * 2));

    for(std::size_t idx = 0; idx < hotbar_.size(); ++idx)
    {
        float slotLeft = slotWidth * static_cast<float>(idx);
        float iconLeft = slotLeft + kHotbarIconPadding;
        float iconRight = iconLeft + iconSize;
        float iconTop = static_cast<float>(m_hotbarHeight) - kHotbarIconPadding;
        float iconBottom = iconTop - iconSize;

        float baseShade = hotbar_[idx].panel && hotbar_[idx].panel->isOpen() ? 0.8f : 0.35f;
        glColor4f(baseShade, baseShade, baseShade + 0.1f, 1.0f);
        glBegin(GL_QUADS);
            glVertex2f(iconLeft, iconBottom);
            glVertex2f(iconRight, iconBottom);
            glVertex2f(iconRight, iconTop);
            glVertex2f(iconLeft, iconTop);
        glEnd();

        glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(iconLeft, iconBottom);
            glVertex2f(iconRight, iconBottom);
            glVertex2f(iconRight, iconTop);
            glVertex2f(iconLeft, iconTop);
        glEnd();
    }
}

void UIService::drawPanels(int width, int height)
{
    if(panels_.empty()) { return; }

    const int margin = 16;
    int currentY = height - m_panelHeight - margin;
    int baseX = width - m_panelWidth - margin;
    if(baseX < 0) { baseX = margin; }

    for(const auto &panel : panels_)
    {
        if(!panel->isOpen()) { continue; }
        panel->render(baseX, currentY, m_panelWidth, m_panelHeight);
        currentY -= (m_panelHeight + margin);
        if(currentY < margin) { break; }
    }
}

void UIService::handleMouseButton(const event::MouseButtonEvent &button)
{
    if(button.button != SDL_BUTTON_LEFT || button.state != SDL_PRESSED) { return; }

    int width = 0;
    int height = 0;
    if(!getWindowSize(width, height) || width == 0 || height == 0) { return; }

    int yFromBottom = height - button.y;
    if(yFromBottom < 0 || yFromBottom > m_hotbarHeight) { return; }

    if(hotbar_.empty()) { return; }

    int slotWidth = std::max(1, width / static_cast<int>(hotbar_.size()));
    int index = button.x / slotWidth;
    if(index < 0) { index = 0; }
    if(index >= static_cast<int>(hotbar_.size())) { index = static_cast<int>(hotbar_.size()) - 1; }

    auto *panel = hotbar_[index].panel;
    if(!panel) { return; }

    printf("Toggled: %s\n", hotbar_[index].panel->title().c_str());
    
    panel->toggle();
}

bool UIService::getWindowSize(int &width, int &height) const
{
    SDL_Window *window = SDL_GL_GetCurrentWindow();
    if(!window)
    {
        width = height = 0;
        return false;
    }

    SDL_GetWindowSize(window, &width, &height);
    return true;
}

} // namespace ui
} // namespace blastmap
