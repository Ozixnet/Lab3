#include "Magic/Hand.h"
#include "Magic/SpellFactory.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <random>
#include <chrono>

// Конструктор
Hand::Hand(size_t maxSize)
    : maxSize(maxSize) {
    // Выдаем SummonSpell в начале игры
    addSpell(SpellFactory::createSpell("SummonSpell"));
    // Выдаем EnhanceSpell в начале игры (для улучшения других заклинаний)
    addSpell(SpellFactory::createSpell("EnhanceSpell"));
    addSpell(SpellFactory::createSpell("TrapSpell"));
    addSpell(SpellFactory::createSpell("AreaDamage"));
    addSpell(SpellFactory::createSpell("SummonSpell"));
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

// Добавить заклинание по имени (для загрузки сохранений)
bool Hand::addSpellByName(const std::string& spellName) {
    auto spell = SpellFactory::createSpell(spellName);
    if (spell) {
        return addSpell(std::move(spell));
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
    
    size_t originalSize = spells.size();
    size_t toRemove = originalSize / 2;
    
    // Если нечего удалять, выйти
    if (toRemove == 0) {
        return;
    }
    
    // Создать список индексов
    std::vector<size_t> indices;
    indices.reserve(originalSize);
    for (size_t i = 0; i < originalSize; ++i) {
        indices.push_back(i);
    }
    
    // Перемешать индексы с использованием современного генератора случайных чисел
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);
    std::shuffle(indices.begin(), indices.end(), generator);
    
    // Выбрать первые toRemove индексов для удаления
    std::vector<size_t> toRemoveIndices;
    toRemoveIndices.reserve(toRemove);
    for (size_t i = 0; i < toRemove; ++i) {
        toRemoveIndices.push_back(indices[i]);
    }
    
    // Отсортировать в обратном порядке, чтобы удалять с конца (не сбивать индексы)
    std::sort(toRemoveIndices.begin(), toRemoveIndices.end(), std::greater<size_t>());
    
    // Удалить выбранные карты (удаляем в обратном порядке, чтобы индексы не сбивались)
    for (size_t idx : toRemoveIndices) {
        // При удалении в обратном порядке индексы всегда валидны
        spells.erase(spells.begin() + idx);
    }
}
