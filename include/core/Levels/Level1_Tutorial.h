#ifndef LEVEL1_TUTORIAL_H
#define LEVEL1_TUTORIAL_H

#include "core/Level.h"

/**
 * @brief Уровень 1: Обучение
 * 
 * Цель: Убить всех врагов
 * Особенности:
 * - Маленькое поле (10x10)
 * - 3 слабых врага (HP: 5, Damage: 1)
 * - Нет башен и зданий
 * - Простой уровень для знакомства с игрой
 */
class Level1_Tutorial : public Level {
public:
    void initialize(EntityManager& em) override;
    bool isVictoryCondition(const EntityManager& em) const override;
    
    std::string getName() const override { 
        return "Level 1: Tutorial"; 
    }
    
    std::string getDescription() const override { 
        return "Убейте всех врагов! Используйте WASD";
    }
    
    int getBoardSize() const override { 
        return 10; 
    }
    
    int getDifficulty() const override { 
        return 1; 
    }
};

#endif // LEVEL1_TUTORIAL_H

