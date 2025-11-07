#ifndef GRID_H
#define GRID_H

#include <vector>
#include "Cell.h"

class Grid {
private:
    int width;
    int height;
    std::vector<std::vector<CellType>> cells;  // <-- ИЗМЕНИТЬ с int на CellType

    std::vector<CellType>& operator[](int index) {
        return cells[index];
    }

    const std::vector<CellType>& operator[](int index) const {
        return cells[index];
    }

public:
    // Конструкторы
    Grid(int w, int h);
    Grid(int size, CellType initialValue);

    // Конструкторы копирования и перемещения
    Grid(const Grid& other) = default;
    Grid(Grid&& other) noexcept = default;
    Grid& operator=(const Grid& other) = default;
    Grid& operator=(Grid&& other) noexcept = default;

    // Геттеры
    int getWidth() const;
    int getHeight() const;
    const Grid& getGrid() const;

    // Методы работы с сеткой
    void setCell(int x, int y, CellType value);
    CellType getCell(int x, int y) const;
    bool isValidPosition(int x, int y) const;
    void clear();
    void resize(int w, int h, CellType initialValue);

    // ТОЛЬКО Border и EntityManager могут использовать operator[]
    friend class Board;
    friend class EntityManager;
};

#endif
