#ifndef LEVEL2_TOWER_DEFENSE_H
#define LEVEL2_TOWER_DEFENSE_H

#include "core/Level.h"

/**
 * @brief Уровень 2: Защита от башен
 * 
 * Цель: Уничтожить все башни и врагов
 * Особенности:
 * - Среднее поле (15x15)
 * - 4 средних врага (HP: 8, Damage: 2)
 * - 2 башни (атакуют заклинаниями)
 * - Нет зданий
 * - Башни представляют серьезную угрозу
 */
class Level2_TowerDefense : public Level {
public:
    void initialize(EntityManager& em) override;
    bool isVictoryCondition(const EntityManager& em) const override;
    
    std::string getName() const override { 
        return "Level 2: Tower Defense"; 
    }
    
    std::string getDescription() const override { 
        return "Уничтожьте все башни! Они стреляют заклинаниями!"; 
    }
    
    int getBoardSize() const override { 
        return 15; 
    }
    
    int getDifficulty() const override { 
        return 3; 
    }
};

#endif // LEVEL2_TOWER_DEFENSE_H

