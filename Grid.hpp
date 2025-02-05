#ifndef GRID_HPP
#define GRID_HPP

#include <unordered_map>
#include <vector>
#include <utility>
#include "MatterGenerator.hpp"

// Визначення хеш-функції для std::pair
struct pair_hash {
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& pair) const {
        auto hash1 = std::hash<T1>{}(pair.first);
        auto hash2 = std::hash<T2>{}(pair.second);
        return hash1 ^ hash2;
    }
};

class Grid {
public:
    Grid(float cellSize) : cellSize(cellSize) {}

    void insert(Matter* matter) {
        auto cell = getCell(matter->shape.getPosition());
        cells[cell].push_back(matter);
    }

    std::vector<Matter*> retrieve(const sf::Vector2f& position) {
        auto cell = getCell(position);
        return cells[cell];
    }

    void clear() {
        cells.clear();
    }

private:
    float cellSize;
    std::unordered_map<std::pair<int, int>, std::vector<Matter*>, pair_hash> cells;

    std::pair<int, int> getCell(const sf::Vector2f& position) {
        return std::make_pair(static_cast<int>(position.x / cellSize), static_cast<int>(position.y / cellSize));
    }
};

#endif 
