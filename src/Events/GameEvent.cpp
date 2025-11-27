#include "Events/GameEvent.h"

#include <iomanip>
#include <sstream>

GameEvent::GameEvent(EventType type)
    : type(type)
    , timestamp(std::chrono::system_clock::now()) {}

PlayerMovedEvent::PlayerMovedEvent(int fromX, int fromY, int toX, int toY)
    : GameEvent(EventType::PLAYER_MOVED)
    , fromX(fromX)
    , fromY(fromY)
    , toX(toX)
    , toY(toY) {}

std::string PlayerMovedEvent::toString() const {
    std::ostringstream oss;
    oss << "[MOVE] Игрок: (" << fromX << "," << fromY << ") → ("
        << toX << "," << toY << ")";
    return oss.str();
}

DamageDealtEvent::DamageDealtEvent(const std::string& attacker,
                                   const std::string& target,
                                   int damage)
    : GameEvent(EventType::DAMAGE_DEALT)
    , attacker(attacker)
    , target(target)
    , damage(damage) {}

std::string DamageDealtEvent::toString() const {
    std::ostringstream oss;
    oss << "[DAMAGE] " << attacker << " нанес " << damage
        << " урона → " << target;
    return oss.str();
}

EntityDiedEvent::EntityDiedEvent(const std::string& entityName, int x, int y)
    : GameEvent(EventType::ENTITY_DIED)
    , entityName(entityName)
    , x(x)
    , y(y) {}

std::string EntityDiedEvent::toString() const {
    std::ostringstream oss;
    oss << "[DEATH] " << entityName << " погиб на ("
        << x << "," << y << ")";
    return oss.str();
}

SpellCastEvent::SpellCastEvent(const std::string& spellName, int x, int y)
    : GameEvent(EventType::SPELL_CAST)
    , spellName(spellName)
    , x(x)
    , y(y) {}

std::string SpellCastEvent::toString() const {
    std::ostringstream oss;
    oss << "[SPELL] '" << spellName << "' использовано на ("
        << x << "," << y << ")";
    return oss.str();
}

LevelStartedEvent::LevelStartedEvent(const std::string& levelName, int levelIndex)
    : GameEvent(EventType::LEVEL_STARTED)
    , levelName(levelName)
    , levelIndex(levelIndex) {}

std::string LevelStartedEvent::toString() const {
    std::ostringstream oss;
    oss << "[LEVEL] Начат уровень " << levelIndex << ": " << levelName;
    return oss.str();
}

LevelCompletedEvent::LevelCompletedEvent(const std::string& levelName,
                                         int levelIndex,
                                         int moves,
                                         int hp)
    : GameEvent(EventType::LEVEL_COMPLETED)
    , levelName(levelName)
    , levelIndex(levelIndex)
    , moves(moves)
    , remainingHp(hp) {}

std::string LevelCompletedEvent::toString() const {
    std::ostringstream oss;
    oss << "[LEVEL] Завершен уровень " << levelIndex << ": " << levelName
        << " | Ходы: " << moves << " | HP: " << remainingHp;
    return oss.str();
}

GameOverEvent::GameOverEvent(int moveCount)
    : GameEvent(EventType::GAME_OVER)
    , moves(moveCount) {}

std::string GameOverEvent::toString() const {
    std::ostringstream oss;
    oss << "[GAME OVER] Поражение на ходу " << moves;
    return oss.str();
}







