# CMakeLists.txt для тестов заклинаний

cmake_minimum_required(VERSION 3.10)

# Название проекта тестов
project(SpellTests)

# Стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включаем директории с заголовочными файлами
include_directories(${CMAKE_SOURCE_DIR}/include)

# Создаем исполняемый файл для тестов
add_executable(test_spells
    test_spells.cpp
    # Добавляем необходимые реализации
    ../src/Magic/Spells/DirectDamage.cpp
    ../src/Magic/Spells/AreaDamage.cpp
    ../src/Magic/Spells/TrapSpell.cpp
    ../src/Magic/Spells/SummonSpell.cpp
    ../src/Magic/Spells/EnhanceSpell.cpp
    ../src/Magic/SpellBuffContext.cpp
    ../src/Board/Trap.cpp
)

# Линковка (если нужна)
# target_link_libraries(test_spells ...)

# Добавляем тест в CTest
enable_testing()
add_test(NAME SpellTests COMMAND test_spells)

