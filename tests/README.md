# Unit-тесты для заклинаний

## Компиляция и запуск тестов

### Вариант 1: Используя CMake (из директории tests)

```bash
cd tests
mkdir build
cd build
cmake ..
cmake --build .
./test_spells
```

### Вариант 2: Прямая компиляция (из директории tests)

```bash
g++ -std=c++17 -I../include \
    test_spells.cpp \
    ../src/Magic/Spells/DirectDamage.cpp \
    ../src/Magic/Spells/AreaDamage.cpp \
    ../src/Magic/Spells/TrapSpell.cpp \
    ../src/Magic/Spells/SummonSpell.cpp \
    ../src/Magic/Spells/EnhanceSpell.cpp \
    ../src/Magic/SpellBuffContext.cpp \
    ../src/Board/Trap.cpp \
    -o test_spells

./test_spells
```

### Вариант 3: Используя основной CMake проекта

Добавьте в основной CMakeLists.txt:

```cmake
# Включаем тесты
enable_testing()
add_subdirectory(tests)
```

Затем:

```bash
cd build
cmake ..
make test_spells
./test_spells
```

## Что тестируется

### DirectDamage
- ✓ Базовое использование с выбором цели
- ✓ Отсутствие целей в радиусе
- ✓ Работа с баффами (увеличение радиуса)
- ✓ Отмена выбора цели
- ✓ Выбор из нескольких целей (враги, здания, башни)

### AreaDamage
- ✓ Базовое использование с выбором области
- ✓ Проверка радиуса действия
- ✓ Проверка границ сетки
- ✓ Отмена заклинания

### TrapSpell
- ✓ Размещение ловушки на свободной клетке
- ✓ Проверка радиуса размещения
- ✓ Проверка занятости клетки
- ✓ Проверка существующих ловушек
- ✓ Проверка границ сетки
- ✓ Отмена размещения

### SummonSpell
- ✓ Призыв одного союзника
- ✓ Призыв нескольких союзников
- ✓ Отсутствие свободного места
- ✓ Работа с баффами (увеличение количества)

### EnhanceSpell
- ✓ Базовое использование
- ✓ Различная мощность усиления
- ✓ Всегда успешное выполнение

### Интеграционные тесты
- ✓ Enhance + DirectDamage (увеличение радиуса)
- ✓ Enhance + TrapSpell (увеличение урона)
- ✓ Несколько заклинаний без Enhance

## Преимущества правильной архитектуры

Благодаря использованию интерфейса `ISpellContext` и `MockSpellContext`:

1. **Не требуется создание сложных объектов**
   - Нет нужды в EntityManager
   - Нет нужды в Board
   - Нет нужды в Player
   - Нет нужды в Grid

2. **Тесты быстрые и изолированные**
   - Выполняются мгновенно
   - Не зависят от других частей системы
   - Легко отлаживать

3. **Легкая настройка тестовых сценариев**
   - Можно задать любую позицию игрока
   - Можно добавить любые цели
   - Можно настроить состояние клеток
   - Можно эмулировать пользовательский ввод

4. **Проверка действий**
   - MockSpellContext логирует все действия
   - Можно проверить, что заклинание сделало правильные вызовы
   - Можно проверить параметры вызовов

## Пример использования Mock

```cpp
MockSpellContext context;
context.setPlayerPosition(5, 5);
context.addTarget(6, 6, TargetInfo::Type::Enemy, 10);
context.setNextTargetSelection(0);

DirectDamage spell(5, 2);
spell.use(context);

// Проверяем, что был нанесен урон
assert(context.hasAction("dealDamage(6, 6, 5)"));
```

## Сравнение: ДО и ПОСЛЕ рефакторинга

### ДО (плохо):
```cpp
// Для теста DirectDamage нужно:
Player player(...);
Board board(10, player);
EntityManager& em = board.getEntityManager();
em.addEnemy(6, 6, 10, 1);

DirectDamage spell(5, 2);
spell.use(em, 10);  // Нужен весь EntityManager!
```

### ПОСЛЕ (хорошо):
```cpp
// Для теста DirectDamage нужно:
MockSpellContext context;
context.addTarget(6, 6, TargetInfo::Type::Enemy, 10);

DirectDamage spell(5, 2);
spell.use(context);  // Только минимальный интерфейс!
```

## Вывод

Правильная инкапсуляция делает код:
- **Тестируемым** - легко писать unit-тесты
- **Понятным** - видно, какие данные нужны заклинанию
- **Безопасным** - заклинание не может испортить EntityManager
- **Гибким** - легко менять реализацию

