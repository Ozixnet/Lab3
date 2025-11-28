cmake_minimum_required(VERSION 3.27)
project(untitled)

# Добавляем cmake в PATH для возможности запуска из консоли
# На Windows: добавьте путь к cmake в системную переменную PATH
# Обычно это: C:\Program Files\CMake\bin или C:\Program Files (x86)\CMake\bin
# Или используйте: set PATH=%PATH%;C:\Program Files\CMake\bin
find_program(CMAKE_EXECUTABLE cmake)
if(CMAKE_EXECUTABLE)
    get_filename_component(CMAKE_BIN_DIR ${CMAKE_EXECUTABLE} DIRECTORY)
    if(WIN32)
        set(ENV{PATH} "$ENV{PATH};${CMAKE_BIN_DIR}")
    else()
        set(ENV{PATH} "$ENV{PATH}:${CMAKE_BIN_DIR}")
    endif()
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

# === СОБРАТЬ ВСЕ ИСХОДНЫЕ ФАЙЛЫ ===
file(GLOB_RECURSE SOURCES
        "src/*.cpp"
)

# === СОЗДАТЬ ИСПОЛНЯЕМЫЙ ФАЙЛ ===
add_executable(game main.cpp ${SOURCES}
        src/Entity/Enemies/EnemyTower.cpp
        include/Entity/Enemies/EnemyTower.h
        src/Magic/Spells/TrapSpell.cpp
        include/Magic/Spells/TrapSpell.h
        src/Magic/TrapRegistry.cpp
        src/Input/ConsoleInputReader.cpp
        src/Input/GuiInputReader.cpp
        src/Rendering/ConsoleRenderer.cpp
        src/Rendering/ImprovedGuiRenderer.cpp
        src/Rendering/GuiRenderer.cpp
        src/UI/GuiMenuSystem.cpp
        # Новые файлы
        src/Entity/Allies/Ally.cpp
        src/Magic/SpellBuffContext.cpp
        src/Magic/Spells/SummonSpell.cpp
        src/Magic/Spells/EnhanceSpell.cpp
        src/Magic/SpellBuffContext.cpp
        src/Board/Grid.cpp
        src/Magic/Spells/SummonSpell.cpp
        src/Magic/GameSpellContext.cpp
        src/Magic/GuiSpellContext.cpp
        include/Magic/ISpellContext.h
        include/Magic/GameSpellContext.h
        include/Magic/GuiSpellContext.h
        include/Magic/MockSpellContext.h
        
        # Система уровней (ver.3.11)
        src/core/Level.cpp
        src/core/LevelManager.cpp
        src/core/Levels/Level1_Tutorial.cpp
        src/core/Levels/Level2_TowerDefense.cpp
        src/core/Levels/Level3_BuildingAssault.cpp
        src/core/Levels/Level4_FinalBattle.cpp
        include/core/Level.h
        include/core/LevelManager.h
        include/core/Levels/Level1_Tutorial.h
        include/core/Levels/Level2_TowerDefense.h
        include/core/Levels/Level3_BuildingAssault.h
        include/core/Levels/Level4_FinalBattle.h
        src/Events/EventBus.cpp
        src/Logging/GameLogger.cpp
)

# Копируем папку config в папку сборки, чтобы игра находила keybindings.cfg
file(COPY ${CMAKE_SOURCE_DIR}/config DESTINATION ${CMAKE_BINARY_DIR})

if(MSVC)
    target_compile_options(game PRIVATE /W4)
else()
    target_compile_options(game PRIVATE -Wall -Wextra -Wpedantic)
endif()



