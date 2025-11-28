#ifndef HAND_H
#define HAND_H

#include "SpellCard.h"
#include <vector>
#include <memory>
#include <string>

class Hand {
public:
    explicit Hand(size_t maxSize);

    // Добавить заклинание
    bool addSpell(std::unique_ptr<SpellCard> spell);
    
    // Добавить заклинание по имени (для загрузки сохранений)
    bool addSpellByName(const std::string& spellName);

    // Добавить случайное заклинание
    void addRandomSpell();

    // Удалить заклинание по индексу
    void removeSpell(size_t index);

    // Получить заклинание
    SpellCard* getSpell(size_t index) const;

    // Информация о руке
    size_t size() const;
    bool isFull() const;
    bool isEmpty() const;

    // Отображение
    void showHand() const;
    
    // Очистить руку
    void clear();
    
    // Удалить половину карт случайным образом
    void removeHalfRandomly();

private:
    std::vector<std::unique_ptr<SpellCard>> spells;
    size_t maxSize;

    // Приватные методы
    void validateIndex(size_t index) const;
};

#endif // HAND_H
