#include "InputService.hpp"

#include <cstdio>
#include <algorithm>

#include "../level/LevelInfo.hpp"

namespace blastmap {
namespace input {
using namespace level;

InputService::InputService(editor::State &editor,
                           view::EditorRenderer &renderer,
                           rom::Rom &rom,
                           event::EventManager &eventManager)
    : m_editor(editor)
    , m_renderer(renderer)
    , m_rom(rom)
    , m_eventManager(eventManager)
{
    std::fill(m_previousKeyState.begin(), m_previousKeyState.end(), 0);
    loadDefaultMappings();
    initializeKeyboardState();
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
            m_eventManager.publish(event::KeyDownEvent{binding.key, binding.scancode});
            if(!handleKey(binding.key)) { return false; }
        }
        m_previousKeyState[binding.scancode] = static_cast<Uint8>(isDown);
    }

    return true;
}

bool InputService::handleKey(SDL_Keycode key)
{
    const unsigned char currentSelectedX = m_renderer.SelectedBlockX();
    const unsigned char currentSelectedY = m_renderer.SelectedBlockY();

    if(key == SDLK_ESCAPE)
    {
        if(m_editor.zoom == editor::ZoomMode::Map) { return false; }
        m_editor.zoomOut();
        return true;
    }

    if(key == SDLK_UP) { m_editor.moveUp(); return true; }
    if(key == SDLK_DOWN) { m_editor.moveDown(); return true; }
    if(key == SDLK_LEFT) { m_editor.moveLeft(); return true; }
    if(key == SDLK_RIGHT) { m_editor.moveRight(); return true; }

    if(key == SDLK_RETURN || key == SDLK_KP_ENTER) { m_editor.zoomIn(); return true; }
    if(key == SDLK_s) { m_rom.save(); return true; }

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

    if(m_editor.zoom == editor::ZoomMode::Screen)
    {
        if(key == SDLK_KP_8 || key == SDLK_RIGHTBRACKET)
        {
            unsigned char block = gLevelManager.blockAt(m_editor.level, m_editor.mode, currentSelectedX, currentSelectedY);
            if(block == 0xFF) { block = 0x00; }
            else if(block == gLevelManager.highestBlockID(m_editor.level, m_editor.mode)) { block = 0x00; }
            else { block++; }
            gLevelManager.setBlockAt(m_editor.level, m_editor.mode, currentSelectedX, currentSelectedY, block);
            return true;
        }

        if(key == SDLK_KP_2 || key == SDLK_LEFTBRACKET)
        {
            unsigned char block = gLevelManager.blockAt(m_editor.level, m_editor.mode, currentSelectedX, currentSelectedY);
            if(block == 0x00) { block = gLevelManager.highestBlockID(m_editor.level, m_editor.mode); }
            else { block--; }
            gLevelManager.setBlockAt(m_editor.level, m_editor.mode, currentSelectedX, currentSelectedY, block);
            return true;
        }

        if(key == SDLK_c)
        {
            gLevelManager.setBlockClipboard(
                gLevelManager.blockAt(m_editor.level, m_editor.mode, currentSelectedX, currentSelectedY)
            );
            return true;
        }

        if(key == SDLK_v)
        {
            gLevelManager.setBlockAt(m_editor.level, m_editor.mode, currentSelectedX, currentSelectedY, gLevelManager.blockClipboard());
            return true;
        }

        if(key == SDLK_KP_PLUS || key == SDLK_EQUALS)
        {
            unsigned short spx = (currentSelectedX * 4) + 1;
            unsigned short spy = (currentSelectedY * 4) + 1;
            gLevelManager.setSpawnPoint(m_editor.level, m_editor.mode, spx, spy);
            return true;
        }

        if(key == SDLK_t)
        {
            unsigned short sbx = (currentSelectedX * 4);
            unsigned short sby = (currentSelectedY * 4);
            for(int tx = 0; tx < 4; ++tx)
            {
                for(int ty = 0; ty < 4; ++ty)
                {
                    short thing = gLevelManager.thingAt(m_editor.level, m_editor.mode, sbx + tx, sby + ty);
                    if(thing >= 0) { std::printf("Thing found: %d\n", thing); }
                    m_editor.thing = static_cast<unsigned short>(thing < 0 ? 0 : thing);
                }
            }
            return true;
        }
    }

    return true;
}

void InputService::loadDefaultMappings()
{
    auto addBinding = [this](const std::string &name, SDL_Keycode key) {
        if(m_keyBindings.emplace(name, key).second)
        {
            monitorKey(key);
        }
    };

    addBinding("ZoomInMain", SDLK_RETURN);
    addBinding("ZoomInPad", SDLK_KP_ENTER);
    addBinding("ZoomOut", SDLK_ESCAPE);
    addBinding("Save", SDLK_s);
    addBinding("MoveUp", SDLK_UP);
    addBinding("MoveDown", SDLK_DOWN);
    addBinding("MoveLeft", SDLK_LEFT);
    addBinding("MoveRight", SDLK_RIGHT);

    // Track additional keys that don't need explicit named bindings
    monitorKey(SDLK_0);
    monitorKey(SDLK_1);
    monitorKey(SDLK_2);
    monitorKey(SDLK_3);
    monitorKey(SDLK_4);
    monitorKey(SDLK_5);
    monitorKey(SDLK_6);
    monitorKey(SDLK_7);
    monitorKey(SDLK_8);
    monitorKey(SDLK_KP_8);
    monitorKey(SDLK_KP_2);
    monitorKey(SDLK_RIGHTBRACKET);
    monitorKey(SDLK_LEFTBRACKET);
    monitorKey(SDLK_c);
    monitorKey(SDLK_v);
    monitorKey(SDLK_KP_PLUS);
    monitorKey(SDLK_EQUALS);
    monitorKey(SDLK_t);
    // TODO: load key mappings from disk/config to allow remapping in the future.
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
