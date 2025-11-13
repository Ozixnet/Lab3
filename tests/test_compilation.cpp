/**
 * @file test_compilation.cpp
 * @brief Простой тест компиляции для проверки ISpellContext
 * 
 * Компиляция:
 * g++ -std=c++17 -I../include -c test_compilation.cpp
 * 
 * Если компилируется без ошибок - всё в порядке!
 */

#include "Magic/ISpellContext.h"
#include "Magic/MockSpellContext.h"
#include "Magic/GameSpellContext.h"
#include <iostream>

// Простая проверка, что все типы определены
void testTypes() {
    // TargetInfo должен компилироваться
    TargetInfo target;
    target.x = 5;
    target.y = 5;
    target.type = TargetInfo::Type::Enemy;
    target.index = 0;
    target.health = 10;
    target.displayName = "Test Target";
    
    std::cout << "TargetInfo скомпилирован успешно!" << std::endl;
    
    // MockSpellContext должен компилироваться
    MockSpellContext mockContext;
    mockContext.setPlayerPosition(5, 5);
    mockContext.addTarget(6, 6, TargetInfo::Type::Enemy, 10);
    
    std::cout << "MockSpellContext скомпилирован успешно!" << std::endl;
    
    // Проверяем, что методы интерфейса работают
    auto pos = mockContext.getPlayerPosition();
    int gridSize = mockContext.getGridSize();
    auto targets = mockContext.findTargetsInRadius(5, 5, 2);
    
    std::cout << "Все методы ISpellContext работают!" << std::endl;
    std::cout << "  Позиция игрока: (" << pos.first << ", " << pos.second << ")" << std::endl;
    std::cout << "  Размер сетки: " << gridSize << std::endl;
    std::cout << "  Найдено целей: " << targets.size() << std::endl;
}

int main() {
    std::cout << "=== ТЕСТ КОМПИЛЯЦИИ ISpellContext ===" << std::endl;
    
    try {
        testTypes();
        std::cout << "\n✅ ВСЁ СКОМПИЛИРОВАНО УСПЕШНО!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ ОШИБКА: " << e.what() << std::endl;
        return 1;
    }
}

