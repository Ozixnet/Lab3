#include "Magic/Hand.h"
#include "Magic/SpellFactory.h"

#include <iostream>
#include <algorithm>
#include <vector>

// Конструктор
Hand::Hand(size_t maxSize)
    : maxSize(maxSize) {
    // Выдаем SummonSpell в начале игры
    addSpell(SpellFactory::createSpell("SummonSpell"));
    // Выдаем EnhanceSpell в начале игры (для улучшения других заклинаний)
    addSpell(SpellFactory::createSpell("EnhanceSpell"));
    addSpell(SpellFactory::createSpell("TrapSpell"));
    addSpell(SpellFactory::createSpell("AreaDamage"));
    // Добавляем случайное заклинание
    //addRandomSpell();
}

// Добавить заклинание, если место есть
bool Hand::addSpell(std::unique_ptr<SpellCard> spell) {
    if (spells.size() < maxSize) {
        spells.push_back(std::move(spell));
        return true;
    }
    return false;
}

// Добавить случайное заклинание из доступного списка
void Hand::addRandomSpell() {
    auto spell = SpellFactory::createRandomSpell();
    if (spell) {
        addSpell(std::move(spell));
    }
}

// Количество заклинаний в руке
size_t Hand::size() const {
    return spells.size();
}

// Получить заклинание по индексу
SpellCard* Hand::getSpell(size_t index) const {
    if (index < spells.size()) {
        return spells[index].get();
    }
    return nullptr;
}

// Удалить заклинание по индексу
void Hand::removeSpell(size_t index) {
    if (index < spells.size()) {
        spells.erase(spells.begin() + index);
    }
}

// Показать все заклинания в руке
void Hand::showHand() const {
    std::cout << "\n=== Ваши заклинания ===" << std::endl;
    for (size_t i = 0; i < spells.size(); ++i) {
        std::cout << "[" << i << "] " << spells[i]->getName() << std::endl;
    }
}

// Проверить, полна ли рука
bool Hand::isFull() const {
    return spells.size() >= maxSize;
}

// Проверить, пуста ли рука
bool Hand::isEmpty() const {
    return spells.empty();
}

// Валидация индекса
void Hand::validateIndex(size_t index) const {
    if (index >= spells.size()) {
        throw std::out_of_range("Индекс заклинания выходит за границы!");
    }
}

// Очистить руку
void Hand::clear() {
    spells.clear();
}

// Удалить половину карт случайным образом
void Hand::removeHalfRandomly() {
    if (spells.empty()) {
        return;
    }
    
    size_t toRemove = spells.size() / 2;
    
    // Создать список индексов
    std::vector<size_t> indices;
    for (size_t i = 0; i < spells.size(); ++i) {
        indices.push_back(i);
    }
    
    // Перемешать индексы
    std::random_shuffle(indices.begin(), indices.end());
    
    // Удалить первые toRemove карт (в обратном порядке, чтобы не сбивать индексы)
    std::sort(indices.begin(), indices.begin() + toRemove, std::greater<size_t>());
    
    for (size_t i = 0; i < toRemove; ++i) {
        if (indices[i] < spells.size()) {
            spells.erase(spells.begin() + indices[i]);
        }
    }
}
