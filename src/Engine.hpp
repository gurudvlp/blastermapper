#ifndef HEADER_ENGINE
#define HEADER_ENGINE

#include <SDL2/SDL.h>

#include <memory>
#include <string>

#include "editor/EditorState.hpp"
#include "event/EventManager.hpp"
#include "service/ServiceRegistry.hpp"
#include "view/EditorRenderer.hpp"

namespace blastmap {

class Engine {
public:
    Engine(view::EditorRenderer &renderer, editor::State &editor);

    void run();
    void registerService(std::unique_ptr<IService> service);
    event::EventManager &eventManager() { return m_eventManager; }
    const event::EventManager &eventManager() const { return m_eventManager; }

    template<typename T>
    T *getServiceAs(const std::string &serviceName)
    {
        return m_serviceRegistry.getServiceAs<T>(serviceName);
    }

private:
    void renderFrame();

    view::EditorRenderer &m_renderer;
    editor::State &m_editor;
    ServiceRegistry m_serviceRegistry;
    event::EventManager m_eventManager;
};

} // namespace blastmap

#endif
