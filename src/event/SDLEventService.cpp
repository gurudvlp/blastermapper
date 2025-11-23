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
        else if(sdlEvent.type == SDL_MOUSEWHEEL)
        {
            m_eventManager.publish(MouseWheelEvent{sdlEvent.wheel.x, sdlEvent.wheel.y});
        }
        else if(sdlEvent.type == SDL_MOUSEBUTTONDOWN || sdlEvent.type == SDL_MOUSEBUTTONUP)
        {
            m_eventManager.publish(MouseButtonEvent{sdlEvent.button.button,
                                                    static_cast<Uint8>(sdlEvent.button.state),
                                                    sdlEvent.button.x,
                                                    sdlEvent.button.y});
        }
        else if(sdlEvent.type == SDL_MOUSEMOTION)
        {
            m_eventManager.publish(MouseMotionEvent{sdlEvent.motion.x,
                                                    sdlEvent.motion.y,
                                                    sdlEvent.motion.xrel,
                                                    sdlEvent.motion.yrel,
                                                    sdlEvent.motion.state});
        }
    }

    return true;
}

} // namespace event
} // namespace blastmap
