#ifndef HEADER_ENGINE
#define HEADER_ENGINE

#include <SDL2/SDL.h>

#include <memory>
#include <string>

#include "editor/EditorState.hpp"
#include "platform/Window.hpp"
#include "rom/Rom.hpp"
#include "service/ServiceRegistry.hpp"
#include "view/EditorRenderer.hpp"

namespace blastmap {

class Engine {
public:
    Engine(platform::Window &window, view::EditorRenderer &renderer, editor::State &editor, rom::Rom &rom);

    void run();
    void registerService(std::unique_ptr<IService> service);

    template<typename T>
    T *getServiceAs(const std::string &serviceName)
    {
        return m_serviceRegistry.getServiceAs<T>(serviceName);
    }

private:
    void renderFrame();
    bool handleWindowEvent(const SDL_Event &event);

    platform::Window &m_window;
    view::EditorRenderer &m_renderer;
    editor::State &m_editor;
    rom::Rom &m_rom;
    ServiceRegistry m_serviceRegistry;
};

} // namespace blastmap

#endif
