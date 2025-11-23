#ifndef HEADER_INPUT_SERVICE
#define HEADER_INPUT_SERVICE

#include <SDL2/SDL.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <unordered_map>
#include <string>
#include <vector>

#include "../editor/EditorState.hpp"
#include "../event/EventManager.hpp"
#include "../rom/Rom.hpp"
#include "../service/ServiceRegistry.hpp"

namespace blastmap {
namespace ui {
class UIService;
}
namespace input {

class InputService : public IService {
public:
    static constexpr const char *kServiceName = "InputService";

    InputService(editor::State &editor,
                 rom::Rom &rom,
                 event::EventManager &eventManager);

    std::string serviceName() const override { return kServiceName; }

    bool tick() override;

    void setUIService(ui::UIService *uiService) { m_uiService = uiService; }

private:
    struct MonitoredKey
    {
        SDL_Keycode key;
        SDL_Scancode scancode;
    };

    void monitorKey(SDL_Keycode key);
    void initializeKeyboardState();

    bool handleKey(SDL_Keycode key);
    void onMouseWheel(const event::MouseWheelEvent &wheel);
    void onMouseButton(const event::MouseButtonEvent &button);
    void onMouseMotion(const event::MouseMotionEvent &motion);

    float screenToNormalizedX(int x, int width) const;
    float screenToNormalizedY(int y, int height) const;
    bool getWindowSize(int &width, int &height) const;
    void clampCameraCenter();

    void loadDefaultMappings();

    editor::State &m_editor;
    rom::Rom &m_rom;
    std::unordered_map<std::string, SDL_Keycode> m_keyBindings;
    std::vector<MonitoredKey> m_monitoredKeys;
    std::array<Uint8, SDL_NUM_SCANCODES> m_previousKeyState{};
    event::EventManager &m_eventManager;
    bool m_rightButtonDown = false;
    float m_dragAnchorX = editor::State::MapHalf;
    float m_dragAnchorY = editor::State::MapHalf;
    static constexpr float kZoomFactor = 1.15f;
    static constexpr float kMinZoom = 0.25f;
    static constexpr float kMaxZoom = 8.0f;
    ui::UIService *m_uiService = nullptr;
};

} // namespace input
} // namespace blastmap

#endif
