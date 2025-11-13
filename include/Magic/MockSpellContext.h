#ifndef MOCK_SPELL_CONTEXT_H
#define MOCK_SPELL_CONTEXT_H

#include "ISpellContext.h"
#include "SpellBuffContext.h"
#include <map>
#include <sstream>

/**
 * @brief Mock реализация ISpellContext для unit-тестов
 * 
 * Эта реализация не требует EntityManager, Board, Player и других классов.
 * Все данные хранятся внутри и могут быть настроены для тестов.
 * Позволяет легко тестировать заклинания без создания всей игровой структуры.
 */
class MockSpellContext : public ISpellContext {
private:
    // Настраиваемые данные для тестов
    std::pair<int, int> playerPosition{5, 5};
    int gridSize{10};
    std::vector<TargetInfo> targets;
    std::map<std::pair<int, int>, bool> emptycells;
    std::map<std::pair<int, int>, bool> traps;
    SpellBuffContext buffContext;
    
    // Для проверки в тестах
    std::vector<std::string> actionsLog;
    int requestedTargetIndex{-1};
    std::pair<int, int> requestedCoordinates{-1, -1};
    
public:
    MockSpellContext() = default;
    
    // === НАСТРОЙКА ДЛЯ ТЕСТОВ ===
    
    void setPlayerPosition(int x, int y) { playerPosition = {x, y}; }
    void setGridSize(int size) { gridSize = size; }
    
    void addTarget(int x, int y, TargetInfo::Type type, int health = 10) {
        TargetInfo target;
        target.x = x;
        target.y = y;
        target.type = type;
        target.index = static_cast<int>(targets.size());
        target.health = health;
        target.displayName = "MockTarget #" + std::to_string(target.index);
        targets.push_back(target);
    }
    
    void setCellEmpty(int x, int y, bool isEmpty) {
        emptycells[{x, y}] = isEmpty;
    }
    
    void setTrapAt(int x, int y, bool hasTrap) {
        traps[{x, y}] = hasTrap;
    }
    
    void setNextTargetSelection(int index) {
        requestedTargetIndex = index;
    }
    
    void setNextCoordinates(int x, int y) {
        requestedCoordinates = {x, y};
    }
    
    // === ПРОВЕРКА В ТЕСТАХ ===
    
    const std::vector<std::string>& getActionsLog() const { return actionsLog; }
    
    void clearActionsLog() { actionsLog.clear(); }
    
    bool hasAction(const std::string& action) const {
        for (const auto& a : actionsLog) {
            if (a.find(action) != std::string::npos) {
                return true;
            }
        }
        return false;
    }
    
    // === РЕАЛИЗАЦИЯ ИНТЕРФЕЙСА ===
    
    std::pair<int, int> getPlayerPosition() const override {
        return playerPosition;
    }
    
    int getGridSize() const override {
        return gridSize;
    }
    
    std::vector<TargetInfo> findTargetsInRadius(int centerX, int centerY, int radius) const override {
        std::vector<TargetInfo> result;
        
        for (const auto& target : targets) {
            int distX = std::abs(target.x - centerX);
            int distY = std::abs(target.y - centerY);
            
            if (distX <= radius && distY <= radius) {
                result.push_back(target);
            }
        }
        
        return result;
    }
    
    bool isCellEmpty(int x, int y) const override {
        auto it = emptycells.find({x, y});
        if (it != emptycells.end()) {
            return it->second;
        }
        // По умолчанию клетка пустая
        return true;
    }
    
    bool hasTrapAt(int x, int y) const override {
        auto it = traps.find({x, y});
        if (it != traps.end()) {
            return it->second;
        }
        return false;
    }
    
    bool dealDamage(int x, int y, int damage) override {
        std::ostringstream oss;
        oss << "dealDamage(" << x << ", " << y << ", " << damage << ")";
        actionsLog.push_back(oss.str());
        
        // Симулируем успешную атаку, если есть цель
        for (auto& target : targets) {
            if (target.x == x && target.y == y && target.health > 0) {
                target.health -= damage;
                return true;
            }
        }
        return false;
    }
    
    bool dealDamageToTarget(const TargetInfo& target, int damage) override {
        return dealDamage(target.x, target.y, damage);
    }
    
    bool placeTrap(int x, int y, int damage) override {
        if (x < 0 || x >= gridSize || y < 0 || y >= gridSize) {
            return false;
        }
        
        if (!isCellEmpty(x, y) || hasTrapAt(x, y)) {
            return false;
        }
        
        std::ostringstream oss;
        oss << "placeTrap(" << x << ", " << y << ", " << damage << ")";
        actionsLog.push_back(oss.str());
        
        traps[{x, y}] = true;
        return true;
    }
    
    bool summonAlly(int x, int y, int health, int damage) override {
        if (x < 0 || x >= gridSize || y < 0 || y >= gridSize) {
            return false;
        }
        
        if (!isCellEmpty(x, y)) {
            return false;
        }
        
        std::ostringstream oss;
        oss << "summonAlly(" << x << ", " << y << ", " << health << ", " << damage << ")";
        actionsLog.push_back(oss.str());
        
        return true;
    }
    
    std::pair<int, int> findFreeAdjacentCell(int centerX, int centerY) const override {
        // Проверяем соседние клетки
        const int dx[] = {-1, 0, 1, -1, 1, -1, 0, 1};
        const int dy[] = {-1, -1, -1, 0, 0, 1, 1, 1};
        
        for (int i = 0; i < 8; ++i) {
            int nx = centerX + dx[i];
            int ny = centerY + dy[i];
            
            if (nx >= 0 && nx < gridSize && ny >= 0 && ny < gridSize) {
                if (isCellEmpty(nx, ny)) {
                    return {nx, ny};
                }
            }
        }
        
        return {-1, -1};
    }
    
    SpellBuffContext& getBuffContext() override {
        return buffContext;
    }
    
    const SpellBuffContext& getBuffContext() const override {
        return buffContext;
    }
    
    int requestTargetSelection(const std::vector<TargetInfo>& targets) const override {
        // Возвращаем предустановленный индекс
        if (requestedTargetIndex >= 0 && requestedTargetIndex < static_cast<int>(targets.size())) {
            return requestedTargetIndex;
        }
        // Или первую цель по умолчанию
        return targets.empty() ? -1 : 0;
    }
    
    std::pair<int, int> requestCoordinates(const std::string& prompt) const override {
        // Возвращаем предустановленные координаты
        return requestedCoordinates;
    }
};

#endif // MOCK_SPELL_CONTEXT_H

