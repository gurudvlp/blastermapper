#include "InputService.hpp"

#include <cmath>

namespace blastmap {
namespace input {

InputService::InputService(editor::State &editor,
                           rom::Rom &rom,
                           event::EventManager &eventManager)
    : m_editor(editor)
    , m_rom(rom)
    , m_eventManager(eventManager)
{
    std::fill(m_previousKeyState.begin(), m_previousKeyState.end(), 0);
    loadDefaultMappings();
    initializeKeyboardState();

    m_eventManager.subscribe<event::MouseWheelEvent>(
        [this](const event::MouseWheelEvent &payload) { onMouseWheel(payload); });
    m_eventManager.subscribe<event::MouseButtonEvent>(
        [this](const event::MouseButtonEvent &payload) { onMouseButton(payload); });
    m_eventManager.subscribe<event::MouseMotionEvent>(
        [this](const event::MouseMotionEvent &payload) { onMouseMotion(payload); });
}

bool InputService::tick()
{
    SDL_PumpEvents();
    const Uint8 *currentState = SDL_GetKeyboardState(nullptr);
    if(!currentState) { return true; }

    for(const auto &binding : m_monitoredKeys)
    {
        bool previouslyDown = m_previousKeyState[binding.scancode];
        bool isDown = currentState[binding.scancode];
        if(isDown && !previouslyDown)
        {
            if(!handleKey(binding.key)) { return false; }
        }
        m_previousKeyState[binding.scancode] = static_cast<Uint8>(isDown);
    }

    return true;
}

bool InputService::handleKey(SDL_Keycode key)
{
    if(key == SDLK_ESCAPE) { return false; }

    if(key == SDLK_s)
    {
        m_rom.save();
        return true;
    }

    if(key >= SDLK_1 && key <= SDLK_8)
    {
        m_editor.level = static_cast<int>(key - SDLK_1);
        return true;
    }

    if(key == SDLK_0)
    {
        m_editor.mode = (m_editor.mode == 0) ? 1 : 0;
        return true;
    }

    return true;
}

void InputService::onMouseWheel(const event::MouseWheelEvent &wheel)
{
    if(wheel.y == 0 && wheel.x == 0) { return; }

    int width = 0;
    int height = 0;
    if(!getWindowSize(width, height) || width == 0 || height == 0) { return; }

    int mouseX = 0;
    int mouseY = 0;
    SDL_GetMouseState(&mouseX, &mouseY);

    float normX = screenToNormalizedX(mouseX, width);
    float normY = screenToNormalizedY(mouseY, height);
    const float previousHalfWorld = editor::State::MapHalf / m_editor.viewZoom;
    const float anchorX = m_editor.viewCenterX + normX * previousHalfWorld;
    const float anchorY = m_editor.viewCenterY + normY * previousHalfWorld;

    float scrollAmount = wheel.y != 0 ? static_cast<float>(wheel.y)
                                       : static_cast<float>(wheel.x);
    float zoomDelta = std::pow(kZoomFactor, scrollAmount);
    float newZoom = std::clamp(m_editor.viewZoom * zoomDelta, kMinZoom, kMaxZoom);
    m_editor.viewZoom = newZoom;

    const float halfWorld = editor::State::MapHalf / m_editor.viewZoom;
    m_editor.viewCenterX = anchorX - normX * halfWorld;
    m_editor.viewCenterY = anchorY - normY * halfWorld;
    clampCameraCenter();
}

void InputService::onMouseButton(const event::MouseButtonEvent &button)
{
    if(button.button != SDL_BUTTON_RIGHT) { return; }

    if(button.state == SDL_PRESSED)
    {
        m_rightButtonDown = true;
        int width = 0;
        int height = 0;
        if(!getWindowSize(width, height) || width == 0 || height == 0) { return; }
        float normX = screenToNormalizedX(button.x, width);
        float normY = screenToNormalizedY(button.y, height);
        const float halfWorld = editor::State::MapHalf / m_editor.viewZoom;
        m_dragAnchorX = m_editor.viewCenterX + normX * halfWorld;
        m_dragAnchorY = m_editor.viewCenterY + normY * halfWorld;
    }
    else if(button.state == SDL_RELEASED)
    {
        m_rightButtonDown = false;
    }
}

void InputService::onMouseMotion(const event::MouseMotionEvent &motion)
{
    if(!m_rightButtonDown) { return; }

    int width = 0;
    int height = 0;
    if(!getWindowSize(width, height) || width == 0 || height == 0) { return; }

    float normX = screenToNormalizedX(motion.x, width);
    float normY = screenToNormalizedY(motion.y, height);
    const float halfWorld = editor::State::MapHalf / m_editor.viewZoom;

    m_editor.viewCenterX = m_dragAnchorX - normX * halfWorld;
    m_editor.viewCenterY = m_dragAnchorY - normY * halfWorld;
    clampCameraCenter();
}

float InputService::screenToNormalizedX(int x, int width) const
{
    if(width == 0) { return 0.0f; }
    return (static_cast<float>(x) / static_cast<float>(width)) * 2.0f - 1.0f;
}

float InputService::screenToNormalizedY(int y, int height) const
{
    if(height == 0) { return 0.0f; }
    return 1.0f - (static_cast<float>(y) / static_cast<float>(height)) * 2.0f;
}

bool InputService::getWindowSize(int &width, int &height) const
{
    SDL_Window * window = SDL_GL_GetCurrentWindow();
    if(!window)
    {
        width = height = 0;
        return false;
    }

    SDL_GetWindowSize(window, &width, &height);
    return true;
}

void InputService::clampCameraCenter()
{
    const float halfWorld = editor::State::MapHalf / m_editor.viewZoom;
    const float mapDimension = editor::State::MapDimension;
    const float mapHalf = editor::State::MapHalf;

    auto clampAxis = [&](float center) {
        if(halfWorld >= mapHalf) { return mapHalf; }
        return std::clamp(center, halfWorld, mapDimension - halfWorld);
    };

    m_editor.viewCenterX = clampAxis(m_editor.viewCenterX);
    m_editor.viewCenterY = clampAxis(m_editor.viewCenterY);
}

void InputService::loadDefaultMappings()
{
    auto addBinding = [this](const std::string &name, SDL_Keycode key) {
        if(m_keyBindings.emplace(name, key).second) { monitorKey(key); }
    };

    addBinding("Exit", SDLK_ESCAPE);
    addBinding("Save", SDLK_s);
    addBinding("Level1", SDLK_1);
    addBinding("Level2", SDLK_2);
    addBinding("Level3", SDLK_3);
    addBinding("Level4", SDLK_4);
    addBinding("Level5", SDLK_5);
    addBinding("Level6", SDLK_6);
    addBinding("Level7", SDLK_7);
    addBinding("Level8", SDLK_8);
    addBinding("ModeToggle", SDLK_0);
}

void InputService::monitorKey(SDL_Keycode key)
{
    SDL_Scancode scancode = SDL_GetScancodeFromKey(key);
    if(scancode == SDL_SCANCODE_UNKNOWN) { return; }

    auto alreadyMonitored =
        std::any_of(m_monitoredKeys.begin(), m_monitoredKeys.end(),
                    [scancode](const MonitoredKey &entry) { return entry.scancode == scancode; });
    if(alreadyMonitored) { return; }

    m_monitoredKeys.push_back({key, scancode});
}

void InputService::initializeKeyboardState()
{
    SDL_PumpEvents();
    const Uint8 *currentState = SDL_GetKeyboardState(nullptr);
    if(!currentState) { return; }

    std::copy(currentState,
              currentState + m_previousKeyState.size(),
              m_previousKeyState.begin());
}

} // namespace input
} // namespace blastmap
