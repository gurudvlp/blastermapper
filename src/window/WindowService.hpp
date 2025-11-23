#ifndef HEADER_WINDOW_SERVICE
#define HEADER_WINDOW_SERVICE

#include <SDL2/SDL.h>
#include <string>

#include "../event/EventManager.hpp"
#include "../platform/Window.hpp"
#include "../service/ServiceRegistry.hpp"

namespace blastmap {
namespace window {

class WindowService : public IService {
public:
    static constexpr const char *kServiceName = "WindowService";

    WindowService(event::EventManager &eventManager);
    ~WindowService();

    std::string serviceName() const override { return kServiceName; }
    bool tick() override { return true; }

    bool initialize();
    void swapBuffers();
    void handleEvent(const SDL_Event &event);
    void teardown();

private:
    platform::Window m_window;
    bool m_initialized;
};

} // namespace window
} // namespace blastmap

#endif
