#include "core/LevelManager.h"
#include "core/Levels/Level1_Tutorial.h"
#include "core/Levels/Level2_TowerDefense.h"
#include "core/Levels/Level3_BuildingAssault.h"
#include "core/Levels/Level4_FinalBattle.h"

LevelManager::LevelManager() 
    : currentLevel(nullptr), currentLevelIndex(-1) 
{
    createLevels();
}

void LevelManager::createLevels() {
    levels.clear();
    
    // Добавить все уровни в порядке прохождения
    levels.push_back(std::make_unique<Level1_Tutorial>());
    levels.push_back(std::make_unique<Level2_TowerDefense>());
    levels.push_back(std::make_unique<Level3_BuildingAssault>());
    levels.push_back(std::make_unique<Level4_FinalBattle>());
}

Level* LevelManager::loadLevel(int index) {
    if (index < 0 || index >= getTotalLevels()) {
        return nullptr;
    }
    
    currentLevelIndex = index;
    currentLevel = levels[index].get();
    return currentLevel;
}

bool LevelManager::nextLevel() {
    if (!hasNextLevel()) {
        return false;
    }
    return loadLevel(currentLevelIndex + 1) != nullptr;
}

void LevelManager::reset() {
    currentLevelIndex = -1;
    currentLevel = nullptr;
}

