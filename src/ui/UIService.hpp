#ifndef BLASTMAP_UI_UISERVICE_HPP
#define BLASTMAP_UI_UISERVICE_HPP

#include <SDL2/SDL.h>

#include "../event/EventManager.hpp"
#include "../service/ServiceRegistry.hpp"
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

    explicit UIService(event::EventManager &eventManager);

    std::string serviceName() const override { return kServiceName; }
    bool tick() override { return true; }

    void render();

private:
    struct HotbarSlot
    {
        std::string label;
        UIPanel *panel = nullptr;
    };

    void drawHotbar(int width, int height);
    void drawPanels(int width, int height);
    void handleMouseButton(const event::MouseButtonEvent &event);
    bool getWindowSize(int &width, int &height) const;

    std::vector<std::unique_ptr<UIPanel>> panels_;
    std::vector<HotbarSlot> hotbar_;
    event::EventManager &m_eventManager;
    int m_hotbarHeight = 64;
    int m_panelWidth = 280;
    int m_panelHeight = 320;
};

} // namespace ui
} // namespace blastmap

#endif // BLASTMAP_UI_UISERVICE_HPP
