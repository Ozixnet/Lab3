#include "Events/EventBus.h"
#include <iostream>

EventBus& EventBus::getInstance() {
    static EventBus instance;
    return instance;
}

void EventBus::subscribe(IEventListener* listener) {
    if (listener) {
        listeners.push_back(listener);
        std::cerr << "[DEBUG EventBus] Подписчик добавлен, всего: " << listeners.size() << "\n";
    }
}

void EventBus::unsubscribe(IEventListener* listener) {
    listeners.erase(std::remove(listeners.begin(), listeners.end(), listener), listeners.end());
}

void EventBus::publish(const GameEvent& event) {
    std::cerr << "[DEBUG EventBus] publish вызван, listeners.size() = " << listeners.size() << "\n";
    for (auto* listener : listeners) {
        if (listener) {
            std::cerr << "[DEBUG EventBus] Вызываю onEvent для listener\n";
            listener->onEvent(event);
        }
    }
}







