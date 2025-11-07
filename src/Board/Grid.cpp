#include "Board/Grid.h"
#include <algorithm>

// Конструктор с шириной и высотой
Grid::Grid(int w, int h) : width(w), height(h) {
    cells.resize(height, std::vector<CellType>(width, CellType::Empty));
}

// ДОБАВИТЬ: новый конструктор для квадратной сетки с начальным значением
Grid::Grid(int size, CellType initialValue) : width(size), height(size) {
    cells.resize(size, std::vector<CellType>(size, initialValue));
}

int Grid::getWidth() const {
    return width;
}

int Grid::getHeight() const {
    return height;
}

void Grid::setCell(int x, int y, CellType value) {
    if (isValidPosition(x, y)) {
        cells[y][x] = value;
    }
}

CellType Grid::getCell(int x, int y) const {
    if (isValidPosition(x, y)) {
        return cells[y][x];
    }
    return CellType::Empty;
}

bool Grid::isValidPosition(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

void Grid::clear() {
    for (auto& row : cells) {
        std::fill(row.begin(), row.end(), CellType::Empty);
    }
}

// ДОБАВИТЬ: метод resize
void Grid::resize(int w, int h, CellType initialValue) {
    width = w;
    height = h;
    cells.resize(height, std::vector<CellType>(width, initialValue));
}
