#pragma once
#include <string>
#include <fstream>
#include <memory>
#include "Core/GameExceptions.h"

// Forward declarations
class GameControl;
class Player;
class Board;
class Hand;
class EntityManager;

// Менеджер сохранения/загрузки игры
class GameSaveManager {
public:
    static constexpr const char* SAVE_FILE = "game_save.dat";
    
    // Сохранить игру
    static void saveGame(const GameControl& game, const std::string& filename = SAVE_FILE);
    
    // Загрузить игру
    static void loadGame(GameControl& game, const std::string& filename = SAVE_FILE);
    
    // Проверить существование файла сохранения
    static bool saveExists(const std::string& filename = SAVE_FILE);
    
    // Удалить сохранение
    static void deleteSave(const std::string& filename = SAVE_FILE);

private:
    // Вспомогательные методы для сериализации
    static void savePlayer(std::ofstream& file, const Player& player);
    static void loadPlayer(std::ifstream& file, Player& player);
    
    static void saveBoard(std::ofstream& file, const Board& board);
    static void loadBoard(std::ifstream& file, Board& board, Player& player);
    
    static void saveEntityManager(std::ofstream& file, const EntityManager& em);
    static void loadEntityManager(std::ifstream& file, EntityManager& em, Board& board, Player& player);
    
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

