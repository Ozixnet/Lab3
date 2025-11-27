#include "Events/EventBus.h"

EventBus& EventBus::getInstance() {
    static EventBus instance;
    return instance;
}

void EventBus::subscribe(IEventListener* listener) {
    if (listener) {
        listeners.push_back(listener);
    }
}

void EventBus::unsubscribe(IEventListener* listener) {
    listeners.erase(std::remove(listeners.begin(), listeners.end(), listener), listeners.end());
}

void EventBus::publish(const GameEvent& event) {
    for (auto* listener : listeners) {
        if (listener) {
            listener->onEvent(event);
        }
    }
}



