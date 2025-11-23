#ifndef HEADER_EVENT_MANAGER
#define HEADER_EVENT_MANAGER

#include <SDL2/SDL.h>

#include <any>
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace blastmap {
namespace event {

struct KeyDownEvent {
    SDL_Keycode key;
    SDL_Scancode scancode;
};

struct MouseWheelEvent {
    int x;
    int y;
};

struct MouseButtonEvent {
    Uint8 button;
    Uint8 state;
    int x;
    int y;
};

struct MouseMotionEvent {
    int x;
    int y;
    int xrel;
    int yrel;
    Uint32 state;
};

struct WindowEvent {
    SDL_Event event;
};

class EventManager {
public:
    template<typename Event>
    using Callback = std::function<void(const Event &)>;

    template<typename Event>
    void subscribe(Callback<Event> callback)
    {
        auto &subscribers = m_subscribers[std::type_index(typeid(Event))];
        subscribers.emplace_back([callback = std::move(callback)](const std::any &payload) {
            callback(std::any_cast<const Event &>(payload));
        });
    }

    template<typename Event>
    void publish(Event event)
    {
        m_eventQueue.emplace_back(std::type_index(typeid(Event)), std::any(std::move(event)));
    }

    void tick()
    {
        for(const auto &queued : m_eventQueue)
        {
            auto it = m_subscribers.find(queued.type);
            if(it == m_subscribers.end()) { continue; }
            for(const auto &subscriber : it->second)
            {
                subscriber(queued.payload);
            }
        }
        m_eventQueue.clear();
    }

private:
    using Subscriber = std::function<void(const std::any &)>;

    struct QueuedEvent {
        QueuedEvent(std::type_index type, std::any payload)
            : type(std::move(type))
            , payload(std::move(payload))
        {
        }

        std::type_index type;
        std::any payload;
    };

    std::vector<QueuedEvent> m_eventQueue;
    std::unordered_map<std::type_index, std::vector<Subscriber>> m_subscribers;
};

} // namespace event
} // namespace blastmap

#endif // HEADER_EVENT_MANAGER
