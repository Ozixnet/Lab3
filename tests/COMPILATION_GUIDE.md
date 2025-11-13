# Руководство по компиляции тестов

## Если возникает ошибка с TargetInfo

Если компилятор жалуется на `TargetInfo` в строке:
```cpp
virtual std::vector<TargetInfo> findTargetsInRadius(...) = 0;
```

### Решение 1: Проверьте порядок includes

Убедитесь, что в файлах заклинаний includes идут в правильном порядке:

```cpp
// Правильный порядок
#include "Magic/Spells/DirectDamage.h"     // Сначала свой заголовок
#include "Magic/ISpellContext.h"            // Потом ISpellContext
#include "Magic/SpellParams.h"
#include "Magic/SpellBuffContext.h"
```

### Решение 2: Убедитесь, что ISpellContext.h подключен

В каждом .cpp файле заклинания должно быть:
```cpp
#include "Magic/ISpellContext.h"
```

### Решение 3: Проверьте путь к include

При компиляции вручную не забудьте указать путь к заголовочным файлам:
```bash
g++ -I../include ...
```

### Решение 4: Проверьте, что структура определена ДО использования

В файле `ISpellContext.h` структура `TargetInfo` должна быть определена ДО класса `ISpellContext`:

```cpp
// ✅ ПРАВИЛЬНО
struct TargetInfo {
    int x;
    int y;
    enum class Type { Enemy, Building, Tower, Ally } type;
    int index;
    int health;
    std::string displayName;
};

class ISpellContext {  // Использует TargetInfo выше
    virtual std::vector<TargetInfo> findTargetsInRadius(...) = 0;
};
```

```cpp
// ❌ НЕПРАВИЛЬНО
class ISpellContext {
    virtual std::vector<TargetInfo> findTargetsInRadius(...) = 0;
};

struct TargetInfo {  // Определена ПОСЛЕ использования!
    // ...
};
```

## Компиляция основного проекта

### Windows (CLion/CMake)
```bash
cd C:\Users\Arkana\CLionProjects\untitled2
cd cmake-build-debug
cmake ..
cmake --build .
```

### Windows (Visual Studio)
```bash
cd C:\Users\Arkana\CLionProjects\untitled2\cmake-build-debug
MSBuild.exe untitled.sln /p:Configuration=Debug
```

### Linux/Mac
```bash
cd build
cmake ..
make
```

## Компиляция только тестов

### Вариант 1: Через CMake
```bash
cd tests
mkdir build
cd build
cmake ..
cmake --build .

# Запуск
./test_spells          # Linux/Mac
test_spells.exe        # Windows
```

### Вариант 2: Вручную (g++)

#### Windows (MinGW/MSYS2):
```bash
cd tests
g++ -std=c++17 -I../include ^
    test_spells.cpp ^
    ../src/Magic/Spells/DirectDamage.cpp ^
    ../src/Magic/Spells/AreaDamage.cpp ^
    ../src/Magic/Spells/TrapSpell.cpp ^
    ../src/Magic/Spells/SummonSpell.cpp ^
    ../src/Magic/Spells/EnhanceSpell.cpp ^
    ../src/Magic/SpellBuffContext.cpp ^
    ../src/Board/Trap.cpp ^
    -o test_spells.exe
```

#### Linux/Mac:
```bash
cd tests
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
```

### Вариант 3: Через основной CMakeLists.txt проекта

Добавьте в конец основного `CMakeLists.txt`:

```cmake
# Тесты
enable_testing()
add_subdirectory(tests)
```

Затем:
```bash
cd cmake-build-debug
cmake ..
cmake --build . --target test_spells
./test_spells
```

## Проверка правильности структуры

Запустите эту команду, чтобы проверить, что все файлы на месте:

### Windows (PowerShell):
```powershell
Get-ChildItem -Recurse include\Magic | Select-Object Name
Get-ChildItem -Recurse src\Magic | Select-Object Name
```

### Linux/Mac:
```bash
find include/Magic -type f
find src/Magic -type f
```

Должны быть файлы:
- ✅ `include/Magic/ISpellContext.h`
- ✅ `include/Magic/GameSpellContext.h`
- ✅ `include/Magic/MockSpellContext.h`
- ✅ `src/Magic/GameSpellContext.cpp`

## Типичные ошибки и решения

### Ошибка: "TargetInfo not declared"
**Причина**: Не подключен `ISpellContext.h`  
**Решение**: Добавьте `#include "Magic/ISpellContext.h"` в .cpp файл

### Ошибка: "incomplete type TargetInfo"
**Причина**: Forward declaration вместо полного определения  
**Решение**: Используйте `#include "Magic/ISpellContext.h"`, а не forward declaration

### Ошибка: "cannot open source file ISpellContext.h"
**Причина**: Неправильный путь к include  
**Решение**: Добавьте `-I../include` или `-I/full/path/to/include`

### Ошибка: "undefined reference to GameSpellContext::..."
**Причина**: Не скомпилирован GameSpellContext.cpp  
**Решение**: Добавьте `src/Magic/GameSpellContext.cpp` в список файлов для компиляции

## Проверка успешности

После компиляции тестов запустите их:
```bash
./test_spells
```

Вы должны увидеть:
```
============================================
     UNIT-ТЕСТЫ ЗАКЛИНАНИЙ
============================================

=== ТЕСТЫ DIRECTDAMAGE ===
Запуск теста: DirectDamage_BasicUse... ✓ ПРОЙДЕН
Запуск теста: DirectDamage_NoTargetsInRadius... ✓ ПРОЙДЕН
...

============================================
     ВСЕ ТЕСТЫ ПРОЙДЕНЫ! ✓
============================================
```

## Нужна помощь?

1. Убедитесь, что все файлы созданы и находятся в правильных директориях
2. Проверьте, что структура `TargetInfo` определена в `ISpellContext.h` ДО класса
3. Проверьте, что все .cpp файлы включают `ISpellContext.h`
4. Используйте CMake - он автоматически разрешает зависимости

