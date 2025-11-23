#include "WindowService.hpp"

#include <GL/gl.h>

namespace blastmap {
namespace window {

WindowService::WindowService(event::EventManager &eventManager)
    : m_initialized(false)
{
    eventManager.subscribe<event::WindowEvent>(
        [this](const event::WindowEvent &payload) { handleEvent(payload.event); });
}

WindowService::~WindowService()
{
    teardown();
}

bool WindowService::initialize()
{
    if(!m_window.initialize()) { return false; }
    m_window.setupGL();
    glViewport(0, 0, m_window.width(), m_window.height());
    m_initialized = true;
    return true;
}

void WindowService::swapBuffers()
{
    if(m_initialized)
    {
        m_window.swapBuffers();
    }
}

void WindowService::handleEvent(const SDL_Event &event)
{
    if(!m_initialized || event.type != SDL_WINDOWEVENT) { return; }
    if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ||
       event.window.event == SDL_WINDOWEVENT_RESIZED)
    {
        m_window.resize(event.window.data1, event.window.data2);
        glViewport(0, 0, m_window.width(), m_window.height());
    }
}

void WindowService::teardown()
{
    if(m_initialized)
    {
        m_window.teardown();
        m_initialized = false;
    }
}

} // namespace window
} // namespace blastmap
