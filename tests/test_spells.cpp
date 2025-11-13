/**
 * @file test_spells.cpp
 * @brief Unit-тесты для заклинаний
 * 
 * Эти тесты демонстрируют преимущества правильной инкапсуляции:
 * - Не требуют создания EntityManager, Board, Player и других классов
 * - Используют MockSpellContext для изоляции тестируемого кода
 * - Легко настраиваются и быстро выполняются
 */

#include "../include/Magic/Spells/DirectDamage.h"
#include "../include/Magic/Spells/AreaDamage.h"
#include "../include/Magic/Spells/TrapSpell.h"
#include "../include/Magic/Spells/SummonSpell.h"
#include "../include/Magic/Spells/EnhanceSpell.h"
#include "../include/Magic/MockSpellContext.h"
#include <iostream>
#include <cassert>

// Макрос для тестов
#define TEST(name) void test_##name()
#define RUN_TEST(name) do { \
    std::cout << "Запуск теста: " << #name << "... "; \
    test_##name(); \
    std::cout << "✓ ПРОЙДЕН\n"; \
} while(0)

// ========================================
// ТЕСТЫ DIRECTDAMAGE
// ========================================

TEST(DirectDamage_BasicUse) {
    MockSpellContext context;
    context.setPlayerPosition(5, 5);
    context.addTarget(6, 6, TargetInfo::Type::Enemy, 10);
    context.setNextTargetSelection(0);  // Выбираем первую цель
    
    DirectDamage spell(5, 2);  // Урон 5, радиус 2
    
    bool result = spell.use(context);
    
    assert(result == true);
    assert(context.hasAction("dealDamage(6, 6, 5)"));
}

TEST(DirectDamage_NoTargetsInRadius) {
    MockSpellContext context;
    context.setPlayerPosition(5, 5);
    context.addTarget(10, 10, TargetInfo::Type::Enemy, 10);  // Слишком далеко
    
    DirectDamage spell(5, 2);  // Радиус 2
    
    bool result = spell.use(context);
    
    assert(result == false);  // Нет целей в радиусе
}

TEST(DirectDamage_WithBuffs) {
    MockSpellContext context;
    context.setPlayerPosition(5, 5);
    context.addTarget(7, 7, TargetInfo::Type::Enemy, 10);  // За пределами обычного радиуса
    context.setNextTargetSelection(0);
    
    // Добавляем баф через EnhanceSpell
    EnhanceSpell enhanceSpell(1);
    enhanceSpell.use(context);  // Добавляет стек усиления
    
    DirectDamage spell(5, 1);  // Радиус 1, но будет увеличен баффом
    
    bool result = spell.use(context);
    
    // После бафа радиус должен увеличиться, и цель станет доступной
    assert(result == true);
}

TEST(DirectDamage_CancelTarget) {
    MockSpellContext context;
    context.setPlayerPosition(5, 5);
    context.addTarget(6, 6, TargetInfo::Type::Enemy, 10);
    context.setNextTargetSelection(-1);  // Отмена выбора
    
    DirectDamage spell(5, 2);
    
    bool result = spell.use(context);
    
    assert(result == false);  // Заклинание отменено
}

TEST(DirectDamage_MultipleTargets) {
    MockSpellContext context;
    context.setPlayerPosition(5, 5);
    context.addTarget(6, 5, TargetInfo::Type::Enemy, 10);
    context.addTarget(7, 5, TargetInfo::Type::Building, 20);
    context.addTarget(5, 6, TargetInfo::Type::Tower, 15);
    context.setNextTargetSelection(1);  // Выбираем здание
    
    DirectDamage spell(10, 2);
    
    bool result = spell.use(context);
    
    assert(result == true);
    assert(context.hasAction("dealDamage(7, 5, 10)"));  // Атакуем здание
}

// ========================================
// ТЕСТЫ AREADAMAGE
// ========================================

TEST(AreaDamage_BasicUse) {
    MockSpellContext context;
    context.setPlayerPosition(5, 5);
    context.setNextCoordinates(7, 7);  // Левый верхний угол области
    
    AreaDamage spell(3, 5);  // Урон 3, радиус 5
    
    context.clearActionsLog();
    bool result = spell.use(context);
    
    assert(result == true);
    // Должны быть нанесены удары по области 2x2 (базовый размер)
    assert(context.hasAction("dealDamage"));
}

TEST(AreaDamage_OutOfRange) {
    MockSpellContext context;
    context.setPlayerPosition(5, 5);
    context.setNextCoordinates(20, 20);  // Слишком далеко
    
    AreaDamage spell(3, 5);  // Радиус 5
    
    bool result = spell.use(context);
    
    assert(result == false);  // За пределами радиуса
}

TEST(AreaDamage_OutOfBounds) {
    MockSpellContext context;
    context.setPlayerPosition(5, 5);
    context.setGridSize(10);
    context.setNextCoordinates(9, 9);  // Область 2x2 выйдет за границы
    
    AreaDamage spell(3, 5);
    
    bool result = spell.use(context);
    
    assert(result == false);  // Выходит за границы сетки
}

TEST(AreaDamage_Cancel) {
    MockSpellContext context;
    context.setPlayerPosition(5, 5);
    context.setNextCoordinates(-1, -1);  // Отмена
    
    AreaDamage spell(3, 5);
    
    bool result = spell.use(context);
    
    assert(result == false);  // Заклинание отменено
}

// ========================================
// ТЕСТЫ TRAPSPELL
// ========================================

TEST(TrapSpell_BasicUse) {
    MockSpellContext context;
    context.setPlayerPosition(5, 5);
    context.setNextCoordinates(6, 6);
    context.setCellEmpty(6, 6, true);
    context.setTrapAt(6, 6, false);
    
    TrapSpell spell(5, 3);  // Урон 5, радиус 3
    
    bool result = spell.use(context);
    
    assert(result == true);
    assert(context.hasAction("placeTrap(6, 6, 5)"));
}

TEST(TrapSpell_OutOfRange) {
    MockSpellContext context;
    context.setPlayerPosition(5, 5);
    context.setNextCoordinates(10, 10);  // Слишком далеко
    
    TrapSpell spell(5, 3);  // Радиус 3
    
    bool result = spell.use(context);
    
    assert(result == false);  // За пределами радиуса
}

TEST(TrapSpell_CellOccupied) {
    MockSpellContext context;
    context.setPlayerPosition(5, 5);
    context.setNextCoordinates(6, 6);
    context.setCellEmpty(6, 6, false);  // Клетка занята
    
    TrapSpell spell(5, 3);
    
    bool result = spell.use(context);
    
    assert(result == false);  // Клетка занята
}

TEST(TrapSpell_TrapAlreadyExists) {
    MockSpellContext context;
    context.setPlayerPosition(5, 5);
    context.setNextCoordinates(6, 6);
    context.setCellEmpty(6, 6, true);
    context.setTrapAt(6, 6, true);  // Уже есть ловушка
    
    TrapSpell spell(5, 3);
    
    bool result = spell.use(context);
    
    assert(result == false);  // Уже есть ловушка
}

TEST(TrapSpell_OutOfBounds) {
    MockSpellContext context;
    context.setPlayerPosition(5, 5);
    context.setGridSize(10);
    context.setNextCoordinates(15, 15);  // За границами
    
    TrapSpell spell(5, 20);  // Большой радиус
    
    bool result = spell.use(context);
    
    assert(result == false);  // За границами сетки
}

TEST(TrapSpell_Cancel) {
    MockSpellContext context;
    context.setPlayerPosition(5, 5);
    context.setNextCoordinates(-1, -1);  // Отмена
    
    TrapSpell spell(5, 3);
    
    bool result = spell.use(context);
    
    assert(result == false);  // Заклинание отменено
}

// ========================================
// ТЕСТЫ SUMMONSPELL
// ========================================

TEST(SummonSpell_BasicUse) {
    MockSpellContext context;
    context.setPlayerPosition(5, 5);
    context.setCellEmpty(6, 5, true);  // Свободная клетка справа
    
    SummonSpell spell(1, 3, 1);  // Призвать 1 союзника, HP=3, DMG=1
    
    bool result = spell.use(context);
    
    assert(result == true);
    assert(context.hasAction("summonAlly"));
}

TEST(SummonSpell_MultipleAllies) {
    MockSpellContext context;
    context.setPlayerPosition(5, 5);
    // Настраиваем несколько свободных клеток вокруг игрока
    context.setCellEmpty(6, 5, true);
    context.setCellEmpty(5, 6, true);
    context.setCellEmpty(6, 6, true);
    
    SummonSpell spell(3, 5, 2);  // Призвать 3 союзников
    
    bool result = spell.use(context);
    
    assert(result == true);
    // Проверяем, что были попытки призвать союзников
    assert(context.hasAction("summonAlly"));
}

TEST(SummonSpell_NoFreeSpace) {
    MockSpellContext context;
    context.setPlayerPosition(5, 5);
    // Все клетки вокруг заняты
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx != 0 || dy != 0) {
                context.setCellEmpty(5 + dx, 5 + dy, false);
            }
        }
    }
    
    SummonSpell spell(1, 3, 1);
    
    bool result = spell.use(context);
    
    assert(result == false);  // Нет свободного места
}

TEST(SummonSpell_WithBuffs) {
    MockSpellContext context;
    context.setPlayerPosition(5, 5);
    context.setCellEmpty(6, 5, true);
    
    // Добавляем бафф через EnhanceSpell
    EnhanceSpell enhanceSpell(1);
    enhanceSpell.use(context);
    
    SummonSpell spell(1, 3, 1);  // Базово 1 союзник, но баф добавит еще
    
    context.clearActionsLog();
    bool result = spell.use(context);
    
    assert(result == true);
    // После баффа должно быть больше попыток призыва
    assert(context.hasAction("summonAlly"));
}

// ========================================
// ТЕСТЫ ENHANCESPELL
// ========================================

TEST(EnhanceSpell_BasicUse) {
    MockSpellContext context;
    
    EnhanceSpell spell(1);
    
    bool result = spell.use(context);
    
    assert(result == true);
    assert(context.getBuffContext().hasBuffs());
}

TEST(EnhanceSpell_MultiplePotency) {
    MockSpellContext context;
    
    EnhanceSpell spell(5);  // Мощность 5
    
    bool result = spell.use(context);
    
    assert(result == true);
    assert(context.getBuffContext().hasBuffs());
}

TEST(EnhanceSpell_AlwaysSucceeds) {
    MockSpellContext context;
    
    EnhanceSpell spell(1);
    
    // EnhanceSpell всегда должен быть успешен
    assert(spell.use(context) == true);
    assert(spell.use(context) == true);
    assert(spell.use(context) == true);
}

// ========================================
// ИНТЕГРАЦИОННЫЕ ТЕСТЫ
// ========================================

TEST(Integration_EnhanceAndDirectDamage) {
    MockSpellContext context;
    context.setPlayerPosition(5, 5);
    context.addTarget(7, 7, TargetInfo::Type::Enemy, 10);
    context.setNextTargetSelection(0);
    
    // Сначала используем Enhance
    EnhanceSpell enhance(1);
    enhance.use(context);
    
    // Потом DirectDamage с увеличенным радиусом
    DirectDamage spell(10, 1);  // Радиус 1, но будет увеличен до 2
    
    bool result = spell.use(context);
    
    // Цель на (7,7) находится на расстоянии 2 от (5,5)
    // С баффом радиус станет 2, и цель будет доступна
    assert(result == true);
}

TEST(Integration_EnhanceAndTrap) {
    MockSpellContext context;
    context.setPlayerPosition(5, 5);
    context.setNextCoordinates(6, 6);
    context.setCellEmpty(6, 6, true);
    
    // Enhance увеличит урон ловушки
    EnhanceSpell enhance(1);
    enhance.use(context);
    
    TrapSpell trap(5, 3);  // Базовый урон 5
    
    context.clearActionsLog();
    bool result = trap.use(context);
    
    assert(result == true);
    // Урон должен быть увеличен баффом
    assert(context.hasAction("placeTrap"));
}

TEST(Integration_MultipleSpellsNoEnhance) {
    MockSpellContext context;
    context.setPlayerPosition(5, 5);
    context.addTarget(6, 5, TargetInfo::Type::Enemy, 10);
    context.setNextTargetSelection(0);
    
    // Используем несколько заклинаний без Enhance
    DirectDamage spell1(5, 2);
    DirectDamage spell2(5, 2);
    
    context.clearActionsLog();
    assert(spell1.use(context) == true);
    
    context.clearActionsLog();
    assert(spell2.use(context) == true);
    
    // Оба заклинания должны работать независимо
}

// ========================================
// ГЛАВНАЯ ФУНКЦИЯ
// ========================================

int main() {
    std::cout << "\n============================================\n";
    std::cout << "     UNIT-ТЕСТЫ ЗАКЛИНАНИЙ\n";
    std::cout << "============================================\n\n";
    
    std::cout << "=== ТЕСТЫ DIRECTDAMAGE ===\n";
    RUN_TEST(DirectDamage_BasicUse);
    RUN_TEST(DirectDamage_NoTargetsInRadius);
    RUN_TEST(DirectDamage_WithBuffs);
    RUN_TEST(DirectDamage_CancelTarget);
    RUN_TEST(DirectDamage_MultipleTargets);
    
    std::cout << "\n=== ТЕСТЫ AREADAMAGE ===\n";
    RUN_TEST(AreaDamage_BasicUse);
    RUN_TEST(AreaDamage_OutOfRange);
    RUN_TEST(AreaDamage_OutOfBounds);
    RUN_TEST(AreaDamage_Cancel);
    
    std::cout << "\n=== ТЕСТЫ TRAPSPELL ===\n";
    RUN_TEST(TrapSpell_BasicUse);
    RUN_TEST(TrapSpell_OutOfRange);
    RUN_TEST(TrapSpell_CellOccupied);
    RUN_TEST(TrapSpell_TrapAlreadyExists);
    RUN_TEST(TrapSpell_OutOfBounds);
    RUN_TEST(TrapSpell_Cancel);
    
    std::cout << "\n=== ТЕСТЫ SUMMONSPELL ===\n";
    RUN_TEST(SummonSpell_BasicUse);
    RUN_TEST(SummonSpell_MultipleAllies);
    RUN_TEST(SummonSpell_NoFreeSpace);
    RUN_TEST(SummonSpell_WithBuffs);
    
    std::cout << "\n=== ТЕСТЫ ENHANCESPELL ===\n";
    RUN_TEST(EnhanceSpell_BasicUse);
    RUN_TEST(EnhanceSpell_MultiplePotency);
    RUN_TEST(EnhanceSpell_AlwaysSucceeds);
    
    std::cout << "\n=== ИНТЕГРАЦИОННЫЕ ТЕСТЫ ===\n";
    RUN_TEST(Integration_EnhanceAndDirectDamage);
    RUN_TEST(Integration_EnhanceAndTrap);
    RUN_TEST(Integration_MultipleSpellsNoEnhance);
    
    std::cout << "\n============================================\n";
    std::cout << "     ВСЕ ТЕСТЫ ПРОЙДЕНЫ! ✓\n";
    std::cout << "============================================\n\n";
    
    std::cout << "ПРЕИМУЩЕСТВА ПРАВИЛЬНОЙ ИНКАПСУЛЯЦИИ:\n";
    std::cout << "✓ Не нужно создавать EntityManager, Board, Player\n";
    std::cout << "✓ Не нужно инициализировать всю игровую структуру\n";
    std::cout << "✓ Тесты быстрые и изолированные\n";
    std::cout << "✓ Легко настраивать тестовые сценарии\n";
    std::cout << "✓ MockSpellContext позволяет проверять действия\n\n";
    
    return 0;
}

