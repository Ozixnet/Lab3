#ifndef LEVEL_H
#define LEVEL_H

#include <string>

// Forward declaration
class EntityManager;

/**
 * @brief Абстрактный базовый класс для уровней игры
 * 
 * Каждый уровень определяет:
 * - Какие враги, башни и здания создать (initialize)
 * - Условие победы (isVictoryCondition)
 * - Информацию об уровне (название, описание, размер)
 */
class Level {
public:
    virtual ~Level() = default;

    /**
     * @brief Инициализировать уровень - создать врагов, башни, здания
     * @param em EntityManager для размещения сущностей на поле
     */
    virtual void initialize(EntityManager& em) = 0;

    /**
     * @brief Проверить, выполнено ли условие победы
     * @param em EntityManager для проверки состояния игры
     * @return true если уровень пройден
     */
    virtual bool isVictoryCondition(const EntityManager& em) const = 0;

    /**
     * @brief Получить название уровня
     * @return Название (например, "Level 1: Tutorial")
     */
    virtual std::string getName() const = 0;

    /**
     * @brief Получить описание уровня
     * @return Описание задачи (например, "Убейте всех врагов!")
     */
    virtual std::string getDescription() const = 0;

    /**
     * @brief Получить размер игрового поля для уровня
     * @return Размер поля (например, 10 для поля 10x10)
     */
    virtual int getBoardSize() const = 0;

    /**
     * @brief Получить сложность уровня
     * @return Сложность от 1 до 5 (по умолчанию 1)
     */
    virtual int getDifficulty() const { return 1; }
};

#endif // LEVEL_H

