#ifndef HEADER_SDL_EVENT_SERVICE
#define HEADER_SDL_EVENT_SERVICE

#include <SDL2/SDL.h>
#include <string>

#include "EventManager.hpp"
#include "../service/ServiceRegistry.hpp"

namespace blastmap {
namespace event {

class SDLEventService : public IService {
public:
    static constexpr const char *kServiceName = "SDLEventService";

    explicit SDLEventService(EventManager &eventManager);

    std::string serviceName() const override { return kServiceName; }

    bool tick() override;

private:
    EventManager &m_eventManager;
};

} // namespace event
} // namespace blastmap

#endif // HEADER_SDL_EVENT_SERVICE
