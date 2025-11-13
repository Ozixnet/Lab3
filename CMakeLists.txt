cmake_minimum_required(VERSION 3.27)
project(untitled)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

# === СОБРАТЬ ВСЕ ИСХОДНЫЕ ФАЙЛЫ ===
file(GLOB_RECURSE SOURCES
        "src/*.cpp"
)

# === СОЗДАТЬ ИСПОЛНЯЕМЫЙ ФАЙЛ ===
add_executable(untitled main.cpp ${SOURCES}
        src/Entity/Enemies/EnemyTower.cpp
        include/Entity/Enemies/EnemyTower.h
        src/Magic/Spells/TrapSpell.cpp
        include/Magic/Spells/TrapSpell.h
        src/Magic/TrapRegistry.cpp
        # Новые файлы
        src/Entity/Allies/Ally.cpp
        src/Magic/SpellBuffContext.cpp
        src/Magic/Spells/SummonSpell.cpp
        src/Magic/Spells/EnhanceSpell.cpp
        src/Magic/SpellBuffContext.cpp
        src/Board/Grid.cpp
        src/Magic/Spells/SummonSpell.cpp
        src/Magic/GameSpellContext.cpp
        include/Magic/ISpellContext.h
        include/Magic/GameSpellContext.h
        include/Magic/MockSpellContext.h
)

if(MSVC)
    target_compile_options(untitled PRIVATE /W4)
else()
    target_compile_options(untitled PRIVATE -Wall -Wextra -Wpedantic)
endif()



