#ifndef HEADER_INPUT_SERVICE
#define HEADER_INPUT_SERVICE

#include <SDL2/SDL.h>
#include <array>
#include <unordered_map>
#include <string>
#include <vector>

#include "../editor/EditorState.hpp"
#include "../rom/Rom.hpp"
#include "../service/ServiceRegistry.hpp"
#include "../view/EditorRenderer.hpp"

namespace blastmap {
namespace input {

class InputService : public IService {
public:
    static constexpr const char *kServiceName = "InputService";

    InputService(editor::State &editor, view::EditorRenderer &renderer, rom::Rom &rom);

    std::string serviceName() const override { return kServiceName; }

    bool tick() override;

private:
    struct MonitoredKey
    {
        SDL_Keycode key;
        SDL_Scancode scancode;
    };

    void monitorKey(SDL_Keycode key);
    void initializeKeyboardState();

    bool handleKey(SDL_Keycode key);

    void loadDefaultMappings();

    editor::State &m_editor;
    view::EditorRenderer &m_renderer;
    rom::Rom &m_rom;
    std::unordered_map<std::string, SDL_Keycode> m_keyBindings;
    std::vector<MonitoredKey> m_monitoredKeys;
    std::array<Uint8, SDL_NUM_SCANCODES> m_previousKeyState{};
};

} // namespace input
} // namespace blastmap

#endif
