#pragma once

#include "Events/GameEvent.h"
#include <algorithm>
#include <vector>

/**
 * @brief Интерфейс для подписчиков на события
 * 
 * Используется в системе логирования (требование 6).
 * 
 * @see GameLogger
 * @see EventBus
 */
class IEventListener {
public:
    virtual ~IEventListener() = default;
    virtual void onEvent(const GameEvent& event) = 0;
};

/**
 * @brief Шина событий (Singleton, паттерн Observer)
 * 
 * Используется для системы логирования (требование 6).
 * Игровые сущности публикуют события, подписчики (GameLogger) реагируют.
 * 
 * @see GameLogger
 * @see GameEvent
 * @see IEventListener
 */
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







