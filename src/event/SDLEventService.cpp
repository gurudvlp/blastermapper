#include "SDLEventService.hpp"

namespace blastmap {
namespace event {

SDLEventService::SDLEventService(EventManager &eventManager)
    : m_eventManager(eventManager)
{
}

bool SDLEventService::tick()
{
    SDL_Event sdlEvent;
    while(SDL_PollEvent(&sdlEvent))
    {
        if(sdlEvent.type == SDL_QUIT)
        {
            return false;
        }

        if(sdlEvent.type == SDL_WINDOWEVENT)
        {
            m_eventManager.publish(WindowEvent{sdlEvent});
        }
        else if(sdlEvent.type == SDL_KEYDOWN)
        {
            m_eventManager.publish(KeyDownEvent{sdlEvent.key.keysym.sym, sdlEvent.key.keysym.scancode});
        }
    }

    return true;
}

} // namespace event
} // namespace blastmap
