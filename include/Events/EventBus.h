#pragma once

#include "Events/GameEvent.h"
#include <algorithm>
#include <vector>

class IEventListener {
public:
    virtual ~IEventListener() = default;
    virtual void onEvent(const GameEvent& event) = 0;
};

class EventBus {
public:
    static EventBus& getInstance();

    void subscribe(IEventListener* listener);
    void unsubscribe(IEventListener* listener);
    void publish(const GameEvent& event);

private:
    EventBus() = default;
    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;

    std::vector<IEventListener*> listeners;
};



