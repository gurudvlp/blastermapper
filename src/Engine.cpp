#include "Engine.hpp"

#include <cstdio>
#include <utility>

#include "ui/UIService.hpp"
#include "window/WindowService.hpp"

namespace blastmap {

Engine::Engine(view::EditorRenderer &renderer, editor::State &editor)
    : m_renderer(renderer)
    , m_editor(editor)
{
}

void Engine::run()
{
    bool running = true;
    auto *windowService = getServiceAs<window::WindowService>(window::WindowService::kServiceName);

    while(running)
    {
        m_serviceRegistry.forEachService([&](IService &service) {
            if(!running) { return false; }
            if(!service.tick())
            {
                running = false;
                return false;
            }
            return true;
        });

        if(!running) { break; }

        m_eventManager.tick();

        renderFrame();
        if(windowService) { windowService->swapBuffers(); }

        SDL_Delay(1);
    }
}

void Engine::renderFrame()
{
    m_renderer.Render(m_editor);
    auto *uiService = getServiceAs<ui::UIService>(ui::UIService::kServiceName);
    if(uiService) { uiService->render(); }

    
}

void Engine::registerService(std::unique_ptr<IService> service)
{
    m_serviceRegistry.registerService(std::move(service));
}

} // namespace blastmap
