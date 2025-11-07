cmake_minimum_required(VERSION 3.27)
project(untitled)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Генерация compile_commands.json для clang-uml
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
)

if(MSVC)
    target_compile_options(untitled PRIVATE /W4)
else()
    target_compile_options(untitled PRIVATE -Wall -Wextra -Wpedantic)
endif()

# === ГЕНЕРАЦИЯ UML ДИАГРАММ ===
# Поиск clang-uml
find_program(CLANG_UML_EXECUTABLE
    NAMES clang-uml
    PATHS
        ${CMAKE_SOURCE_DIR}
        $ENV{PATH}
    DOC "Path to clang-uml executable"
)

if(CLANG_UML_EXECUTABLE)
    message(STATUS "Found clang-uml: ${CLANG_UML_EXECUTABLE}")
    
    # Создаем директорию для диаграмм
    file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/diagrams)
    
    # Кастомная цель для генерации UML
    add_custom_target(generate_uml
        COMMAND ${CLANG_UML_EXECUTABLE}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating UML diagrams with clang-uml..."
        VERBATIM
    )
    
    # Автоматически генерировать UML после компиляции
    add_custom_command(TARGET untitled POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Generating UML diagrams..."
        COMMAND ${CLANG_UML_EXECUTABLE}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Auto-generating UML diagrams after build"
    )
    
    message(STATUS "UML generation enabled. Run 'cmake --build . --target generate_uml' to generate manually.")
else()
    message(WARNING "clang-uml not found. UML generation will be skipped.")
    message(STATUS "To enable UML generation, install clang-uml and ensure it's in your PATH.")
endif()

