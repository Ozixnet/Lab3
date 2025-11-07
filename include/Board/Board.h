#ifndef BORDER_H
#define BORDER_H

#include <vector>
#include "Cell.h"
#include "../Entity/EntityManager.h"
#include "Trap.h"
#include "Grid.h"

class Board {
public:
    explicit Board(int s, Player& p);

    Board(const Board& other);
    Board(Board&& other) noexcept;
    Board& operator=(const Board& other);
    Board& operator=(Board&& other) noexcept;

    ~Board() = default;

    // ТОЛЬКО отображение и вспомогательные методы
    void displayBoard() const;
    void addSlowTraps(int count);

    // Геттеры
    int getSize() const noexcept { return size; }
    EntityManager& getEntityManager() noexcept { return entityManager; }
    const EntityManager& getEntityManager() const noexcept { return entityManager; }
    Grid& getGrid() noexcept { return grid; }
    const Grid& getGrid() const noexcept { return grid; }

    void updateGridForDisplay();


private:
    int size{};
    Player& player;

    Grid grid;
    EntityManager entityManager;
    std::vector<std::pair<int,int>> slowTraps;
};

#endif // BORDER_H
