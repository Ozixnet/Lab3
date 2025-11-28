#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include <string>
#include <utility>
#include <vector>
#include "Enemies/Enemy.h"
#include "Enemies/EnemyBuilding.h"
#include "Enemies/EnemyTower.h"
#include "Allies/Ally.h"
#include "Player/Player.h"
#include "Magic/Spells/TrapSpell.h"
#include "Magic/SpellBuffContext.h"
#include "Input/Direction.h"


class Board;  // Forward declaration

class EntityManager {

public:
    struct AttackInfo {
        bool hit{false};
        bool targetDestroyed{false};
        std::string targetName;
        int targetX{0};
        int targetY{0};
        int damage{0};
    };

    EntityManager(Board* boardPtr, int gridSize, Player* playerPtr);

    // === КОНСТРУКТОРЫ КОПИРОВАНИЯ (только деклараторы!) ===
    EntityManager(const EntityManager& other);
    EntityManager(EntityManager&& other) noexcept;
    EntityManager& operator=(const EntityManager& other);
    EntityManager& operator=(EntityManager&& other) noexcept;

    ~EntityManager() = default;


    // === МЕТОДЫ ДЛЯ ИГРОКА ===
    int playerMove(Direction dir);
    int playerAttack(char key);  // TODO: заменить на Direction в будущем
    void playerPosted();
    void playerReduceHealth(int amount);
    int playerGetHealth() const;

    // === МЕТОДЫ ДЛЯ ВРАГОВ ===
    void addEnemy(int x, int y, int health, int damage);
    void removeEnemy(int index);
    int moveEnemy(int index);
    void moveAllEnemies();
    int findEnemyAtPosition(int x, int y) const;
    int getEnemyCount() const noexcept;
    const std::vector<Enemy>& getEnemies() const;
    const std::vector<std::pair<int,int>>& getEnemyCoords() const;
    bool isEnemyAt(int x, int y) const;
    int getEnemyDamageAt(int x, int y) const;
    std::pair<std::vector<int>, std::vector<int>> enemyScanner(int x, int y, int gridSize) const;

    // === МЕТОДЫ ДЛЯ ЗДАНИЙ ===
    void addEnemyBuilding(int x, int y, int spawnInterval);
    int spawnEnemyNearBuilding(int buildingX, int buildingY);
    int getActiveBuildingCount() const noexcept;
    int getAliveEnemyCount() const noexcept;
    void removeBuilding(int index);
    void processBuildingSpawns();
    int findBuildingAtPosition(int x, int y) const;
    int getBuildingCount() const noexcept;
    const std::vector<EnemyBuilding>& getBuildings() const;

    // === ИГРОВАЯ ЛОГИКА ===
    int handleAttackAt(int x, int y, int damage);

    // === МЕТОДЫ ДЛЯ БАШЕН ===
    void addEnemyTower(int x, int y, int health = 30);
    void removeEnemyTower(int index);
    const std::vector<EnemyTower>& getTowers() const;
    int getTowerCount() const noexcept;
    int findTowerAtPosition(int x, int y) const;

    // Обновить все башни (вызывать каждый ход)
    void updateAllTowers();

    // Проверить атаку башен на игрока
    int checkTowerAttack(int playerX, int playerY);

    // === МЕТОДЫ ДЛЯ ЛОВУШЕК ===
    int checkTrapDamage(int x, int y);  // Возвращает урон ловушки если есть


    // === ЛОВУШКИ ===
    void addSlowTrap(int x, int y);
    void setSlowTraps(const std::vector<std::pair<int,int>>& traps);
    bool isSlowTrap(int x, int y) const;
    void checkEnemyTraps();
    // хук, который должен вызываться при успешном перемещении игрока
    void onPlayerMovedFromTo(int oldX, int oldY, int newX, int newY);




    // API для ловушек игрока
    void armPlayerTrapAfterMove();                    //  пометить, что после ухода надо поставить ловушку
    void placePlayerTrapAt(int x, int y);             // поставить ловушку в координате (x,y)
    bool isPlayerTrapAt(int x, int y) const;          //  проверка
    void removePlayerTrapAt(int x, int y);            //  удалить при срабатывании
    const std::vector<std::pair<int,int>>& getPlayerTraps() const;  // ← геттер для отображения

    // === ГЕТТЕРЫ КООРДИНАТ ИГРОКА ===
    std::pair<int, int> getPlayerCoord() const { return plCoord; }
    void setPlayerCoord(int x, int y) { plCoord = {x, y}; }
    
    // Геттер для доступа к Board (для Spell)
    Board& getBoard() { return *board; }
    const Board& getBoard() const { return *board; }
    
    // Геттер для доступа к SpellBuffContext
    SpellBuffContext& getBuffContext() { return buffContext; }
    const SpellBuffContext& getBuffContext() const { return buffContext; }

    const AttackInfo& getLastPlayerAttackInfo() const { return lastPlayerAttackInfo; }
    
    // === МЕТОДЫ ДЛЯ СОЮЗНИКОВ ===
    void addAlly(int x, int y, int health, int damage);
    void removeAlly(int index);
    int moveAlly(int index);  // Внутренний метод для движения одного союзника
    void moveAllies();
    int findAllyAtPosition(int x, int y) const;
    int getAllyCount() const noexcept;
    const std::vector<Ally>& getAllies() const;
    const std::vector<std::pair<int,int>>& getAllyCoords() const;
    bool isAllyAt(int x, int y) const;
    
    // Утилита для поиска свободной клетки рядом с игроком
    std::pair<int, int> findFreeAdjacentCell(int centerX, int centerY) const;

private:
    Board* board;
    int gridSize;
    Player* player;
    std::pair<int, int> plCoord{1, 1};  // <-- КООРДИНАТЫ ИГРОКА
    std::vector<std::pair<int,int>> playerTraps;     // оординаты ловушек игрока
    int playerTrapDamage = 1;                         // урон ловушки
    bool pendingTrapArm = false;                      // поставить ловушку после ухода


    std::vector<Enemy> enemies;
    std::vector<std::pair<int,int>> enemyCoords;
    std::vector<EnemyBuilding> buildings;
    std::vector<EnemyTower> towers;
    std::vector<Ally> allies;
    std::vector<std::pair<int,int>> allyCoords;
    std::vector<std::pair<int,int>> slowTraps;
    
    SpellBuffContext buffContext;  // Контекст усилений заклинаний

    int nextEnemyID{1};
    int nextBuildingID{1};
    int nextAllyID{1};

    AttackInfo lastPlayerAttackInfo;
    bool recordingPlayerAttack{false};

    void beginPlayerAttackRecord();
    void endPlayerAttackRecord();
    void recordPlayerAttackHit(const std::string& targetName, int x, int y, int damage, bool destroyed);
};

#endif // ENTITYMANAGER_H
