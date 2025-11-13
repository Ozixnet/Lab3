#include "core/GameSaveManager.h"
#include "core/GameControl.h"
#include "Entity/Player/Player.h"
#include "Board/Board.h"
#include "Entity/EntityManager.h"
#include "Magic/Hand.h"
#include "Magic/SpellCard.h"
#include "Magic/Spells/DirectDamage.h"
#include "Magic/Spells/AreaDamage.h"
#include "Magic/Spells/TrapSpell.h"
#include "Magic/Spells/SummonSpell.h"
#include "Magic/Spells/EnhanceSpell.h"
#include "Magic/SpellFactory.h"
#include "core/LevelManager.h"
#include "core/Level.h"

#include <fstream>
#include <iostream>
#include <cstring>

// Магическая сигнатура файла сохранения
constexpr const char* SAVE_SIGNATURE = "GAME_SAVE_V1";
constexpr size_t SIGNATURE_SIZE = 13;

void GameSaveManager::writeHeader(std::ofstream& file) {
    file.write(SAVE_SIGNATURE, SIGNATURE_SIZE);
    if (!file.good()) {
        throw SaveException("Failed to write file header", "writeHeader");
    }
}

bool GameSaveManager::checkHeader(std::ifstream& file) {
    char signature[SIGNATURE_SIZE + 1] = {0};
    file.read(signature, SIGNATURE_SIZE);
    
    if (!file.good() || file.gcount() != SIGNATURE_SIZE) {
        return false;
    }
    
    return std::strcmp(signature, SAVE_SIGNATURE) == 0;
}

void GameSaveManager::saveGame(const GameControl& game, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary | std::ios::trunc);
    
    if (!file.is_open()) {
        throw SaveException("Cannot open file for writing: " + filename, "saveGame");
    }
    
    try {
        // Записать заголовок
        writeHeader(file);
        
        // Сохранить countMove
        writeBinary(file, game.getCountMove());
        
        // Сохранить текущий уровень
        int currentLevelIndex = game.getLevelManager().getCurrentLevelIndex();
        writeBinary(file, currentLevelIndex);
        
        // Сохранить Player
        const Player* player = game.getPlayer();
        if (player) {
            savePlayer(file, *player);
        } else {
            throw SaveException("Player is null", "saveGame");
        }
        
        // Сохранить Board и EntityManager
        const Board* board = game.getBoard();
        if (board) {
            // Сохранить размер поля
            writeBinary(file, board->getSize());
            // Сохранить EntityManager
            saveEntityManager(file, board->getEntityManager());
        } else {
            throw SaveException("Board is null", "saveGame");
        }
        
        // Сохранить Hand
        const Hand* hand = game.getHand();
        if (hand) {
            saveHand(file, *hand);
        } else {
            throw SaveException("Hand is null", "saveGame");
        }
        
        file.close();
        
        if (!file.good() && !file.eof()) {
            throw SaveException("Error occurred while writing file", "saveGame");
        }
        
    } catch (const SaveException&) {
        file.close();
        throw;  // Пробросить дальше
    } catch (const std::exception& e) {
        file.close();
        throw SaveException("Unexpected error: " + std::string(e.what()), "saveGame");
    }
}

void GameSaveManager::loadGame(GameControl& game, const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    
    if (!file.is_open()) {
        throw FileNotFoundException(filename, "loadGame");
    }
    
    try {
        // Проверить заголовок
        if (!checkHeader(file)) {
            throw InvalidDataException("Invalid file format or corrupted save file", "loadGame");
        }
        
        // Загрузить countMove
        int countMove;
        readBinary(file, countMove);
        game.setCountMove(countMove);
        
        // Загрузить текущий уровень
        int currentLevelIndex;
        readBinary(file, currentLevelIndex);
        
        // Загрузить уровень
        LevelManager& lm = game.getLevelManager();
        Level* level = lm.loadLevel(currentLevelIndex);
        if (!level) {
            throw InvalidDataException("Invalid level index: " + std::to_string(currentLevelIndex), "loadGame");
        }
        
        // Загрузить Player
        Player* player = game.getPlayer();
        if (!player) {
            // Создать игрока если его нет
            game.setPlayer(std::make_unique<Player>(1000, 5));
            player = game.getPlayer();
        }
        loadPlayer(file, *player);
        
        // Загрузить размер Board и создать Board
        int boardSize;
        readBinary(file, boardSize);
        
        if (boardSize < 10 || boardSize > 25) {
            throw InvalidDataException("Invalid board size: " + std::to_string(boardSize), "loadGame");
        }
        
        // Создать Board нужного размера
        game.setBoard(std::make_unique<Board>(boardSize, *player));
        Board* board = game.getBoard();
        
        // Загрузить EntityManager (координаты игрока загрузятся внутри)
        loadEntityManager(file, board->getEntityManager());
        
        // Загрузить Hand
        Hand* hand = game.getHand();
        if (!hand) {
            game.setHand(std::make_unique<Hand>(5));
            hand = game.getHand();
        }
        loadHand(file, *hand);
        
        file.close();
        
        if (!file.good() && !file.eof()) {
            throw LoadException("Error occurred while reading file", "loadGame");
        }
        
    } catch (const LoadException&) {
        file.close();
        throw;  // Пробросить дальше
    } catch (const std::exception& e) {
        file.close();
        throw LoadException("Unexpected error: " + std::string(e.what()), "loadGame");
    }
}

bool GameSaveManager::saveExists(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    bool valid = checkHeader(file);
    file.close();
    return valid;
}

void GameSaveManager::deleteSave(const std::string& filename) {
    if (std::remove(filename.c_str()) != 0) {
        throw SaveException("Cannot delete save file: " + filename, "deleteSave");
    }
}

void GameSaveManager::savePlayer(std::ofstream& file, const Player& player) {
    writeBinary(file, player.GetHealth());
    writeBinary(file, player.GetMaxHealth());
    writeBinary(file, player.GetBaseDamage());
    writeBinary(file, player.GetDistance());
    writeBinary(file, player.IsSlowed());
    writeBinary(file, player.IsDoubleDistance());
    writeBinary(file, player.GetUpgradePoints());
}

void GameSaveManager::loadPlayer(std::ifstream& file, Player& player) {
    int health, maxHealth, baseDamage, distance, upgradePoints;
    bool slowed, doubleDistance;
    
    readBinary(file, health);
    readBinary(file, maxHealth);
    readBinary(file, baseDamage);
    readBinary(file, distance);
    readBinary(file, slowed);
    readBinary(file, doubleDistance);
    readBinary(file, upgradePoints);
    
    // Восстановить состояние игрока
    player.SetHealth(health);
    // maxHealth можно установить только через конструктор, поэтому игнорируем
    player.SetBaseDamage(baseDamage);
    player.SetDistance(distance);
    player.SetSlowed(slowed);
    player.SetUpgradePoints(upgradePoints);
    if (doubleDistance && distance == 1) {
        player.ToggleDoubleDistance();  // Переключить если нужно
    }
}

void GameSaveManager::saveEntityManager(std::ofstream& file, const EntityManager& em) {
    // Сохранить координаты игрока
    auto [px, py] = em.getPlayerCoord();
    writeBinary(file, px);
    writeBinary(file, py);
    
    // Сохранить врагов
    size_t enemyCount = em.getEnemyCount();
    writeBinary(file, enemyCount);
    const auto& enemies = em.getEnemies();
    const auto& enemyCoords = em.getEnemyCoords();
    for (size_t i = 0; i < enemyCount; ++i) {
        if (i < enemies.size() && enemies[i].IsAlive()) {
            writeBinary(file, enemyCoords[i].first);
            writeBinary(file, enemyCoords[i].second);
            writeBinary(file, enemies[i].GetHealth());
            writeBinary(file, enemies[i].GetDamage());
        }
    }
    
    // Сохранить союзников
    size_t allyCount = em.getAllyCount();
    writeBinary(file, allyCount);
    const auto& allies = em.getAllies();
    const auto& allyCoords = em.getAllyCoords();
    for (size_t i = 0; i < allyCount; ++i) {
        if (i < allies.size() && allies[i].IsAlive()) {
            writeBinary(file, allyCoords[i].first);
            writeBinary(file, allyCoords[i].second);
            writeBinary(file, allies[i].GetHealth());
            writeBinary(file, allies[i].GetDamage());
        }
    }
    
    // Сохранить башни
    size_t towerCount = em.getTowerCount();
    writeBinary(file, towerCount);
    const auto& towers = em.getTowers();
    for (size_t i = 0; i < towerCount; ++i) {
        if (i < towers.size() && towers[i].isAlive()) {
            writeBinary(file, towers[i].getX());
            writeBinary(file, towers[i].getY());
            writeBinary(file, towers[i].getHealth());
            writeBinary(file, towers[i].getCooldown());
        }
    }
    
    // Сохранить здания
    size_t buildingCount = em.getBuildingCount();
    writeBinary(file, buildingCount);
    const auto& buildings = em.getBuildings();
    for (size_t i = 0; i < buildingCount; ++i) {
        if (i < buildings.size() && buildings[i].isActive()) {
            writeBinary(file, buildings[i].getX());
            writeBinary(file, buildings[i].getY());
            writeBinary(file, buildings[i].getHealth());
            writeBinary(file, buildings[i].getSpawnInterval());
            writeBinary(file, buildings[i].getCurrentTimer());
        }
    }
    
    // Сохранить ловушки игрока
    const auto& playerTraps = em.getPlayerTraps();
    size_t trapCount = playerTraps.size();
    writeBinary(file, trapCount);
    for (const auto& trap : playerTraps) {
        writeBinary(file, trap.first);
        writeBinary(file, trap.second);
    }
}

void GameSaveManager::loadEntityManager(std::ifstream& file, EntityManager& em) {
    // Загрузить координаты игрока
    int px, py;
    readBinary(file, px);
    readBinary(file, py);
    em.setPlayerCoord(px, py);
    
    // Загрузить врагов
    size_t enemyCount;
    readBinary(file, enemyCount);
    
    if (enemyCount > 1000) {  // Защита от некорректных данных
        throw InvalidDataException("Too many enemies: " + std::to_string(enemyCount), "loadEntityManager");
    }
    
    for (size_t i = 0; i < enemyCount; ++i) {
        int x, y, health, damage;
        readBinary(file, x);
        readBinary(file, y);
        readBinary(file, health);
        readBinary(file, damage);
        
        // Проверка корректности данных
        if (x < 0 || y < 0 || health < 0 || damage < 0) {
            throw InvalidDataException("Invalid enemy data at index " + std::to_string(i), "loadEntityManager");
        }
        
        em.addEnemy(x, y, health, damage);
    }
    
    // Загрузить союзников
    size_t allyCount;
    readBinary(file, allyCount);
    
    if (allyCount > 1000) {
        throw InvalidDataException("Too many allies: " + std::to_string(allyCount), "loadEntityManager");
    }
    
    for (size_t i = 0; i < allyCount; ++i) {
        int x, y, health, damage;
        readBinary(file, x);
        readBinary(file, y);
        readBinary(file, health);
        readBinary(file, damage);
        
        if (x < 0 || y < 0 || health < 0 || damage < 0) {
            throw InvalidDataException("Invalid ally data at index " + std::to_string(i), "loadEntityManager");
        }
        
        em.addAlly(x, y, health, damage);
    }
    
    // Загрузить башни
    size_t towerCount;
    readBinary(file, towerCount);
    
    if (towerCount > 100) {
        throw InvalidDataException("Too many towers: " + std::to_string(towerCount), "loadEntityManager");
    }
    
    for (size_t i = 0; i < towerCount; ++i) {
        int x, y, health, cooldown;
        readBinary(file, x);
        readBinary(file, y);
        readBinary(file, health);
        readBinary(file, cooldown);  // Читаем, но не используем (восстановится автоматически)
        
        if (x < 0 || y < 0 || health < 0) {
            throw InvalidDataException("Invalid tower data at index " + std::to_string(i), "loadEntityManager");
        }
        
        em.addEnemyTower(x, y, health);
    }
    
    // Загрузить здания
    size_t buildingCount;
    readBinary(file, buildingCount);
    
    if (buildingCount > 100) {
        throw InvalidDataException("Too many buildings: " + std::to_string(buildingCount), "loadEntityManager");
    }
    
    for (size_t i = 0; i < buildingCount; ++i) {
        int x, y, health, interval, timer;
        readBinary(file, x);
        readBinary(file, y);
        readBinary(file, health);
        readBinary(file, interval);
        readBinary(file, timer);  // Читаем, но не используем (восстановится автоматически)
        
        if (x < 0 || y < 0 || health < 0 || interval < 0) {
            throw InvalidDataException("Invalid building data at index " + std::to_string(i), "loadEntityManager");
        }
        
        em.addEnemyBuilding(x, y, interval);
    }
    
    // Загрузить ловушки игрока
    size_t trapCount;
    readBinary(file, trapCount);
    
    if (trapCount > 1000) {
        throw InvalidDataException("Too many traps: " + std::to_string(trapCount), "loadEntityManager");
    }
    
    for (size_t i = 0; i < trapCount; ++i) {
        int x, y;
        readBinary(file, x);
        readBinary(file, y);
        
        if (x < 0 || y < 0) {
            throw InvalidDataException("Invalid trap data at index " + std::to_string(i), "loadEntityManager");
        }
        
        em.placePlayerTrapAt(x, y);
    }
}

void GameSaveManager::saveHand(std::ofstream& file, const Hand& hand) {
    size_t handSize = hand.size();
    writeBinary(file, handSize);
    
    for (size_t i = 0; i < handSize; ++i) {
        const SpellCard* spell = hand.getSpell(i);
        if (spell) {
            // Сохранить тип заклинания
            std::string spellName = spell->getName();
            size_t nameLen = spellName.length();
            writeBinary(file, nameLen);
            file.write(spellName.c_str(), nameLen);
            
            if (!file.good()) {
                throw SaveException("Failed to write spell name", "saveHand");
            }
        }
    }
}

void GameSaveManager::loadHand(std::ifstream& file, Hand& hand) {
    size_t handSize;
    readBinary(file, handSize);
    
    // Очистить текущую руку
    hand.clear();
    
    // Проверка на разумный размер
    if (handSize > 10) {
        throw InvalidDataException("Hand size too large: " + std::to_string(handSize), "loadHand");
    }
    
    for (size_t i = 0; i < handSize; ++i) {
        size_t nameLen;
        readBinary(file, nameLen);
        
        if (nameLen > 100) {  // Защита от некорректных данных
            throw InvalidDataException("Spell name too long: " + std::to_string(nameLen), "loadHand");
        }
        
        std::string spellName(nameLen, '\0');
        file.read(&spellName[0], nameLen);
        
        if (!file.good()) {
            throw LoadException("Failed to read spell name", "loadHand");
        }
        
        // Создать заклинание по имени
        auto spell = SpellFactory::createSpell(spellName);
        if (spell) {
            hand.addSpell(std::move(spell));
        }
    }
}

