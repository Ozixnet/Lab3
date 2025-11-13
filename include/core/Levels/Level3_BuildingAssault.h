#ifndef LEVEL3_BUILDING_ASSAULT_H
#define LEVEL3_BUILDING_ASSAULT_H

#include "core/Level.h"

/**
 * @brief Уровень 3: Штурм вражеской базы
 * 
 * Цель: Уничтожить вражескую базу (здание)
 * Особенности:
 * - Большое поле (20x20)
 * - 3 начальных врага (HP: 10, Damage: 2)
 * - 1 башня для защиты базы
 * - 1 здание (спавнит врага каждые 5 ходов)
 * - Нужно уничтожить здание быстро, пока не появилось слишком много врагов
 */
class Level3_BuildingAssault : public Level {
public:
    void initialize(EntityManager& em) override;
    bool isVictoryCondition(const EntityManager& em) const override;
    
    std::string getName() const override { 
        return "Level 3: Building Assault"; 
    }
    
    std::string getDescription() const override { 
        return "Уничтожьте вражескую базу! Она создает новых врагов!"; 
    }
    
    int getBoardSize() const override { 
        return 20; 
    }
    
    int getDifficulty() const override { 
        return 4; 
    }
};

#endif // LEVEL3_BUILDING_ASSAULT_H

