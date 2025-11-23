#include "Engine.hpp"

#include <cstdio>
#include <utility>
#include <GL/gl.h>

namespace blastmap {

Engine::Engine(platform::Window &window,
               view::EditorRenderer &renderer,
               editor::State &editor,
               rom::Rom &rom)
    : m_window(window)
    , m_renderer(renderer)
    , m_editor(editor)
    , m_rom(rom)
{
}

void Engine::run()
{
    bool running = true;
    SDL_Event sdlEvent;

    while(running)
    {
        while(SDL_PollEvent(&sdlEvent))
        {
            if(sdlEvent.type == SDL_QUIT)
            {
                running = false;
                break;
            }

            if(sdlEvent.type == SDL_WINDOWEVENT)
            {
                handleWindowEvent(sdlEvent);
            }
        }

        if(!running) { break; }

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

        renderFrame();
        m_window.swapBuffers();

        SDL_Delay(1);
    }
}

void Engine::renderFrame()
{
    m_renderer.Render(m_editor);
}

bool Engine::handleWindowEvent(const SDL_Event &event)
{
    if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ||
       event.window.event == SDL_WINDOWEVENT_RESIZED)
    {
        m_window.resize(event.window.data1, event.window.data2);
        glViewport(0, 0, m_window.width(), m_window.height());
    }
    return true;
}

void Engine::registerService(std::unique_ptr<IService> service)
{
    m_serviceRegistry.registerService(std::move(service));
}

} // namespace blastmap
