#ifndef LEVEL4_FINAL_BATTLE_H
#define LEVEL4_FINAL_BATTLE_H

#include "core/Level.h"

/**
 * @brief Уровень 4: Финальная битва
 * 
 * Цель: Уничтожить ВСЁ!
 * Особенности:
 * - Огромное поле (25x25)
 * - 5 сильных врагов (HP: 15, Damage: 3)
 * - 3 башни
 * - 2 здания (спавнят врагов каждые 3 хода)
 * - Самый сложный уровень в игре
 * - Требуется стратегия и использование заклинаний
 */
class Level4_FinalBattle : public Level {
public:
    void initialize(EntityManager& em) override;
    bool isVictoryCondition(const EntityManager& em) const override;
    
    std::string getName() const override { 
        return "Level 4: Final Battle"; 
    }
    
    std::string getDescription() const override { 
        return "ФИНАЛЬНАЯ БИТВА! Уничтожьте всех врагов, башни и базы!"; 
    }
    
    int getBoardSize() const override { 
        return 25; 
    }
    
    int getDifficulty() const override { 
        return 5; 
    }
};

#endif // LEVEL4_FINAL_BATTLE_H

