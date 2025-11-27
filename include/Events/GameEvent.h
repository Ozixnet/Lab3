#pragma once

#include <chrono>
#include <string>

enum class EventType {
    PLAYER_MOVED,
    DAMAGE_DEALT,
    ENTITY_DIED,
    SPELL_CAST,
    ITEM_PICKED_UP,
    LEVEL_STARTED,
    LEVEL_COMPLETED,
    GAME_OVER
};

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







