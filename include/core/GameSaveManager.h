#pragma once
#include <string>
#include <fstream>
#include <memory>
#include "Board/Board.h"
#include "Entity/EntityManager.h"
#include "Entity/Player/Player.h"
#include "Magic/Hand.h"
#include "core/GameExceptions.h"
#include "core/Level.h"
#include "core/LevelManager.h"

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
        if (!checkHeader(file)) {
            throw InvalidDataException("Invalid file format or corrupted save file", "loadGame");
        }
        
        int countMove;
        readBinary(file, countMove);
        game.setCountMove(countMove);
        
        int currentLevelIndex;
        readBinary(file, currentLevelIndex);
        
        LevelManager& lm = game.getLevelManager();
        Level* level = lm.loadLevel(currentLevelIndex);
        if (!level) {
            throw InvalidDataException("Invalid level index: " + std::to_string(currentLevelIndex), "loadGame");
        }
        
        Player* player = game.getPlayer();
        if (!player) {
            game.setPlayer(std::make_unique<Player>(1000, 5));
            player = game.getPlayer();
        }
        loadPlayer(file, *player);
        
        int boardSize;
        readBinary(file, boardSize);
        if (boardSize < 10 || boardSize > 25) {
            throw InvalidDataException("Invalid board size: " + std::to_string(boardSize), "loadGame");
        }
        
        game.setBoard(std::make_unique<Board>(boardSize, *player));
        Board* board = game.getBoard();
        loadEntityManager(file, board->getEntityManager());
        
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
        throw;
    } catch (const std::exception& e) {
        file.close();
        throw LoadException("Unexpected error: " + std::string(e.what()), "loadGame");
    }
}

