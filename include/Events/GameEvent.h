#pragma once

#include <chrono>
#include <string>

enum class EventType {
    PLAYER_MOVED,      ///< Игрок переместился
    DAMAGE_DEALT,      ///< Нанесён урон
    ENTITY_DIED,       ///< Сущность умерла
    SPELL_CAST,        ///< Заклинание применено
    ITEM_PICKED_UP,    ///< Предмет подобран
    LEVEL_STARTED,    ///< Уровень начался
    LEVEL_COMPLETED,   ///< Уровень пройден
    GAME_OVER          ///< Игра окончена
};

/**
 * @brief Базовый класс для всех игровых событий
 * 
 * Используется в системе логирования (требование 6).
 * Игровые сущности публикуют события через EventBus.
 * 
 * @see EventBus
 * @see GameLogger
 * @see PlayerMovedEvent
 * @see DamageDealtEvent
 * @see SpellCastEvent
 */
class GameEvent {
public:
    explicit GameEvent(EventType type);
    virtual ~GameEvent() = default;

    EventType getType() const { return type; }
    std::chrono::system_clock::time_point getTimestamp() const { return timestamp; }

    virtual std::string toString() const = 0;

private:
    EventType type;
    std::chrono::system_clock::time_point timestamp;
};

/**
 * @brief Событие движения игрока
 * 
 * Публикуется MoveAction в EventBus для логирования (требование 6).
 * 
 * @see GameEvent
 * @see MoveAction
 * @see EventBus
 */
class PlayerMovedEvent : public GameEvent {
public:
    PlayerMovedEvent(int fromX, int fromY, int toX, int toY);
    std::string toString() const override;

private:
    int fromX;
    int fromY;
    int toX;
    int toY;
};

/**
 * @brief Событие нанесения урона
 * 
 * Публикуется AttackAction в EventBus для логирования (требование 6).
 * 
 * @see GameEvent
 * @see AttackAction
 * @see EventBus
 */
class DamageDealtEvent : public GameEvent {
public:
    DamageDealtEvent(const std::string& attacker,
                     const std::string& target,
                     int damage);
    std::string toString() const override;

private:
    std::string attacker;
    std::string target;
    int damage;
};

class EntityDiedEvent : public GameEvent {
public:
    EntityDiedEvent(const std::string& entityName, int x, int y);
    std::string toString() const override;

private:
    std::string entityName;
    int x;
    int y;
};

/**
 * @brief Событие применения заклинания
 * 
 * Публикуется SpellAction в EventBus для логирования (требование 6).
 * 
 * @see GameEvent
 * @see SpellAction
 * @see EventBus
 */
class SpellCastEvent : public GameEvent {
public:
    SpellCastEvent(const std::string& spellName, int x, int y);
    std::string toString() const override;

private:
    std::string spellName;
    int x;
    int y;
};

class LevelStartedEvent : public GameEvent {
public:
    LevelStartedEvent(const std::string& levelName, int levelIndex);
    std::string toString() const override;

private:
    std::string levelName;
    int levelIndex;
};

class LevelCompletedEvent : public GameEvent {
public:
    LevelCompletedEvent(const std::string& levelName, int levelIndex, int moves, int hp);
    std::string toString() const override;

private:
    std::string levelName;
    int levelIndex;
    int moves;
    int remainingHp;
};

class GameOverEvent : public GameEvent {
public:
    explicit GameOverEvent(int moveCount);
    std::string toString() const override;

private:
    int moves;
};







