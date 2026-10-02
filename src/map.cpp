#include "map.h"
#include <cstddef>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

void Map::load(const std::filesystem::path &path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("Cannot open map: " +
                                 static_cast<std::string>(path));
    }

    int width = 0;
    int height = 0;

    constexpr int maxDimension = 1024;

    if (!(file >> width >> height) || width <= 0 || height <= 0 ||
        width > maxDimension || height > maxDimension) {
        throw std::runtime_error("Invalid map dimensions");
    }

    std::vector<int> tiles(static_cast<std::size_t>(width) * height);

    for (int &tile : tiles) {
        if (!(file >> tile)) {
            throw std::runtime_error("Missing or invalid map cell");
        }

        if (tile < 0 || tile > 8) {
            throw std::runtime_error("Map cell must be between 0 and 8");
        }
    }

    file >> std::ws;
    if (!file.eof()) {
        throw std::runtime_error("Unexpected data after map cells");
    }

    width_ = width;
    height_ = height;
    tiles_ = std::move(tiles);
}

int Map::width() const { return width_; }

int Map::height() const { return height_; }

bool Map::contains(int x, int y) const {
    return x >= 0 && x < width_ && y >= 0 && y < height_;
}

int Map::tile(int x, int y) const {
    if (!contains(x, y)) {
        throw std::out_of_range("Map coordinates out of bounds");
    }

    return tiles_[static_cast<std::size_t>(y) * width_ + x];
}

bool Map::isWall(int x, int y) const {
    return !contains(x, y) || tile(x, y) != 0;
}