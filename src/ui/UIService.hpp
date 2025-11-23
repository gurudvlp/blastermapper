#ifndef BLASTMAP_UI_UISERVICE_HPP
#define BLASTMAP_UI_UISERVICE_HPP

#include <SDL2/SDL.h>

#include "../editor/EditorState.hpp"
#include "../event/EventManager.hpp"
#include "../level/LevelInfo.hpp"
#include "../service/ServiceRegistry.hpp"
#include "BlockPanel.hpp"
#include "DummyPanel.hpp"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace blastmap {
namespace ui {

class UIService : public IService
{
public:
    static constexpr const char *kServiceName = "UIService";

    UIService(event::EventManager &eventManager,
              editor::State &editor,
              level::LevelManager &levelManager);

    std::string serviceName() const override { return kServiceName; }
    bool tick() override { return true; }

    void render();
    bool isPointOverPanel(int mouseX, int yFromBottom) const;

private:
    struct HotbarSlot
    {
        std::string label;
        UIPanel *panel = nullptr;
    };

    void drawHotbar(int width, int height);
    void drawPanels(int width, int height);
    void handleMouseButton(const event::MouseButtonEvent &event);
    void handleMouseWheel(const event::MouseWheelEvent &event);
    bool getWindowSize(int &width, int &height) const;

    std::vector<std::unique_ptr<UIPanel>> panels_;
    std::vector<HotbarSlot> hotbar_;
    struct PanelLayout
    {
        UIPanel *panel = nullptr;
        int x = 0;
        int y = 0;
        int width = 0;
        int height = 0;
    };
    std::vector<PanelLayout> panelLayouts_;
    event::EventManager &m_eventManager;
    editor::State &m_editor;
    level::LevelManager &m_levelManager;
    int m_hotbarHeight = 64;
    int m_panelWidth = 280;
    int m_panelHeight = 320;
};

} // namespace ui
} // namespace blastmap

#endif // BLASTMAP_UI_UISERVICE_HPP
