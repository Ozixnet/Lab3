#pragma once
#include <string>
#include <fstream>
#include <memory>
#include <vector>
#include "Board/Board.h"
#include "Entity/EntityManager.h"
#include "Entity/Player/Player.h"
#include "Magic/Hand.h"
#include "core/GameExceptions.h"
#include "core/Level.h"
#include "core/LevelManager.h"

/**
 * @brief Структура для временного хранения данных сохранения
 * 
 * Используется для безопасной загрузки: сначала все данные читаются
 * во временные структуры, валидируются, и только потом применяются.
 * Это предотвращает неконсистентное состояние при ошибке в середине загрузки.
 */
struct SaveData {
    // Основные данные
    int countMove{0};
    int levelIndex{0};
    int boardSize{0};
    
    // Данные игрока
    int playerHealth{0};
    int playerMaxHealth{0};
    int playerDamage{0};
    bool playerIsRanged{false};
    bool playerIsSlowed{false};
    
    // Данные EntityManager (упрощённо — координаты и состояния)
    std::pair<int, int> playerCoord{0, 0};
    
    struct EnemyData {
        int x, y, health, damage, id;
        bool isActive;
    };
    std::vector<EnemyData> enemies;
    
    struct BuildingData {
        int x, y, spawnInterval, spawnTimer, id;
        bool isActive;
    };
    std::vector<BuildingData> buildings;
    
    struct TowerData {
        int x, y, health, id;
        bool isActive;
    };
    std::vector<TowerData> towers;
    
    struct AllyData {
        int x, y, health, damage, id;
        bool isActive;
    };
    std::vector<AllyData> allies;
    
    std::vector<std::pair<int, int>> slowTraps;
    std::vector<std::pair<int, int>> playerTraps;
    
    // Данные Hand
    std::vector<std::string> spellNames;
    
    /**
     * @brief Валидация загруженных данных
     * @return true если данные корректны, false если нет
     */
    bool validate() const {
        if (countMove < 0) return false;
        if (levelIndex < 0 || levelIndex > 10) return false;
        if (boardSize < 10 || boardSize > 25) return false;
        if (playerHealth < 0 || playerMaxHealth <= 0) return false;
        if (playerDamage < 0) return false;
        if (playerCoord.first < 0 || playerCoord.second < 0) return false;
        if (playerCoord.first >= boardSize || playerCoord.second >= boardSize) return false;
        return true;
    }
};

// Менеджер сохранения/загрузки игры
class GameSaveManager {
public:
    static constexpr const char* SAVE_FILE = "game_save.dat";
    
    template<typename TController>
    static void saveGame(const TController& game, const std::string& filename = SAVE_FILE);
    
    template<typename TController>
    static void loadGame(TController& game, const std::string& filename = SAVE_FILE);
    
    // Проверить существование файла сохранения
    static bool saveExists(const std::string& filename = SAVE_FILE);
    
    // Удалить сохранение
    static void deleteSave(const std::string& filename = SAVE_FILE);

private:
    // === БЕЗОПАСНАЯ ЗАГРУЗКА (Read-Validate-Apply) ===
    
    // Шаг 1: Чтение всех данных во временную структуру
    static SaveData readAllData(std::ifstream& file);
    
    // Шаг 2: Применение данных к игре (только после успешной валидации)
    template<typename TController>
    static void applyData(TController& game, const SaveData& data);

    // Вспомогательные методы для сериализации
    static void savePlayer(std::ofstream& file, const Player& player);
    static void loadPlayer(std::ifstream& file, Player& player);
    
    static void saveEntityManager(std::ofstream& file, const EntityManager& em);
    static void loadEntityManager(std::ifstream& file, EntityManager& em);
    
    static void saveHand(std::ofstream& file, const Hand& hand);
    static void loadHand(std::ifstream& file, Hand& hand);
    
    // Вспомогательные методы для записи/чтения примитивов
    template<typename T>
    static void writeBinary(std::ofstream& file, const T& value) {
        file.write(reinterpret_cast<const char*>(&value), sizeof(T));
        if (!file.good()) {
            throw SaveException("Failed to write binary data", "writeBinary");
        }
    }
    
    template<typename T>
    static void readBinary(std::ifstream& file, T& value) {
        file.read(reinterpret_cast<char*>(&value), sizeof(T));
        if (!file.good()) {
            throw LoadException("Failed to read binary data", "readBinary");
        }
    }
    
    // Проверка целостности файла
    static void writeHeader(std::ofstream& file);
    static bool checkHeader(std::ifstream& file);
};

template<typename TController>
void GameSaveManager::saveGame(const TController& game, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary | std::ios::trunc);
    
    if (!file.is_open()) {
        throw SaveException("Cannot open file for writing: " + filename, "saveGame");
    }
    
    try {
        writeHeader(file);
        writeBinary(file, game.getCountMove());
        
        int currentLevelIndex = game.getLevelManager().getCurrentLevelIndex();
        writeBinary(file, currentLevelIndex);
        
        const Player* player = game.getPlayer();
        if (!player) {
            throw SaveException("Player is null", "saveGame");
        }
        savePlayer(file, *player);
        
        const Board* board = game.getBoard();
        if (!board) {
            throw SaveException("Board is null", "saveGame");
        }
        writeBinary(file, board->getSize());
        saveEntityManager(file, board->getEntityManager());
        
        const Hand* hand = game.getHand();
        if (!hand) {
            throw SaveException("Hand is null", "saveGame");
        }
        saveHand(file, *hand);
        
        file.close();
        
        if (!file.good() && !file.eof()) {
            throw SaveException("Error occurred while writing file", "saveGame");
        }
    } catch (const SaveException&) {
        file.close();
        throw;
    } catch (const std::exception& e) {
        file.close();
        throw SaveException("Unexpected error: " + std::string(e.what()), "saveGame");
    }
}

template<typename TController>
void GameSaveManager::loadGame(TController& game, const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    
    if (!file.is_open()) {
        throw FileNotFoundException(filename, "loadGame");
    }
    
    try {
        // === ШАГ 1: ЧТЕНИЕ ВСЕХ ДАННЫХ (без изменения состояния игры) ===
        if (!checkHeader(file)) {
            throw InvalidDataException("Invalid file format or corrupted save file", "loadGame");
        }
        
        SaveData data = readAllData(file);
        file.close();
        
        // === ШАГ 2: ВАЛИДАЦИЯ (до применения изменений) ===
        if (!data.validate()) {
            throw InvalidDataException("Save data validation failed", "loadGame");
        }
        
        // Проверяем, что уровень существует
        LevelManager& lm = game.getLevelManager();
        if (!lm.isValidLevelIndex(data.levelIndex)) {
            throw InvalidDataException("Invalid level index: " + std::to_string(data.levelIndex), "loadGame");
        }
        
        // === ШАГ 3: АТОМАРНОЕ ПРИМЕНЕНИЕ (все или ничего) ===
        applyData(game, data);
        
    } catch (const LoadException&) {
        file.close();
        throw;
    } catch (const std::exception& e) {
        file.close();
        throw LoadException("Unexpected error: " + std::string(e.what()), "loadGame");
    }
}

template<typename TController>
void GameSaveManager::applyData(TController& game, const SaveData& data) {
    // Создаём все объекты заново — это гарантирует консистентность
    LevelManager& lm = game.getLevelManager();
    Level* level = lm.loadLevel(data.levelIndex);
    if (!level) {
        throw LoadException("Failed to load level", "applyData");
    }
    
    // Создаём игрока с загруженными параметрами
    auto newPlayer = std::make_unique<Player>(data.playerMaxHealth, data.playerDamage);
    newPlayer->SetHealth(data.playerHealth);
    if (data.playerIsRanged) {
        newPlayer->SetDistance(2);  // Дальний бой
    }
    newPlayer->SetSlowed(data.playerIsSlowed);
    game.setPlayer(std::move(newPlayer));
    
    // Создаём доску
    Player* player = game.getPlayer();
    auto newBoard = std::make_unique<Board>(data.boardSize, *player);
    EntityManager& em = newBoard->getEntityManager();
    
    // Устанавливаем координаты игрока
    em.setPlayerCoord(data.playerCoord.first, data.playerCoord.second);
    newBoard->getGrid().setCell(data.playerCoord.first, data.playerCoord.second, CellType::Player);
    
    // Загружаем врагов
    for (const auto& e : data.enemies) {
        if (e.isActive) {
            em.addEnemy(e.x, e.y, e.health, e.damage);
        }
    }
    
    // Загружаем здания
    for (const auto& b : data.buildings) {
        if (b.isActive) {
            em.addEnemyBuilding(b.x, b.y, b.spawnInterval);
        }
    }
    
    // Загружаем башни
    for (const auto& t : data.towers) {
        if (t.isActive) {
            em.addEnemyTower(t.x, t.y, t.health);
        }
    }
    
    // Загружаем союзников
    for (const auto& a : data.allies) {
        if (a.isActive) {
            em.addAlly(a.x, a.y, a.health, a.damage);
        }
    }
    
    // Загружаем ловушки
    em.setSlowTraps(data.slowTraps);
    for (const auto& trap : data.playerTraps) {
        em.placePlayerTrapAt(trap.first, trap.second);
    }
    
    game.setBoard(std::move(newBoard));
    
    // Создаём руку заклинаний
    auto newHand = std::make_unique<Hand>(5);
    for (const auto& spellName : data.spellNames) {
        newHand->addSpellByName(spellName);
    }
    game.setHand(std::move(newHand));
    
    // Устанавливаем счётчик ходов в самом конце
    game.setCountMove(data.countMove);
}

