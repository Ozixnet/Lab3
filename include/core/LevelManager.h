#ifndef LEVEL_MANAGER_H
#define LEVEL_MANAGER_H

#include <vector>
#include <memory>
#include "Level.h"

/**
 * @brief Менеджер уровней игры
 * 
 * Управляет:
 * - Созданием всех уровней
 * - Переключением между уровнями
 * - Текущим прогрессом игрока
 */
class LevelManager {
public:
    LevelManager();
    ~LevelManager() = default;
    
    // Запретить копирование (из-за unique_ptr)
    LevelManager(const LevelManager&) = delete;
    LevelManager& operator=(const LevelManager&) = delete;
    
    // Разрешить перемещение
    LevelManager(LevelManager&&) noexcept = default;
    LevelManager& operator=(LevelManager&&) noexcept = default;

    /**
     *  Загрузить уровень по индексу
     *  index Индекс уровня (0-based: 0 = Level 1, 1 = Level 2, и т.д.)
     * return Указатель на загруженный уровень или nullptr если индекс неверный
     */
    Level* loadLevel(int index);

    /**
     *  Перейти к следующему уровню
     * return true если есть следующий уровень и он загружен
     */
    bool nextLevel();

    /**
     *  Сбросить прогресс (вернуться к первому уровню)
     */
    void reset();

    /**
     *  Получить текущий уровень
     * return Указатель на текущий уровень или nullptr
     */
    Level* getCurrentLevel() const { return currentLevel; }

    /**
     *  Получить индекс текущего уровня
     * return Индекс (0-based) или -1 если уровень не загружен
     */
    int getCurrentLevelIndex() const { return currentLevelIndex; }

    /**
     *  Получить общее количество уровней
     * return Количество уровней в игре
     */
    int getTotalLevels() const { return static_cast<int>(levels.size()); }

    /**
     *  Проверить, есть ли следующий уровень
     * return true если есть уровень после текущего
     */
    bool hasNextLevel() const { 
        return currentLevelIndex >= 0 && currentLevelIndex < getTotalLevels() - 1; 
    }

    /**
     *  Проверить, является ли текущий уровень последним
     * return true если это последний уровень
     */
    bool isLastLevel() const { 
        return currentLevelIndex == getTotalLevels() - 1; 
    }

    /**
     *  Проверить, является ли индекс уровня валидным
     * @param index Индекс уровня для проверки
     * return true если индекс валиден (в диапазоне доступных уровней)
     */
    bool isValidLevelIndex(int index) const {
        return index >= 0 && index < getTotalLevels();
    }

private:
    std::vector<std::unique_ptr<Level>> levels;  // Все уровни игры
    Level* currentLevel;                          // Текущий активный уровень
    int currentLevelIndex;                        // Индекс текущего уровня

    /**
     * Создать все уровни игры
     * Вызывается в конструкторе
     */
    void createLevels();
};

#endif // LEVEL_MANAGER_H

