#pragma once

#include <filesystem>
#include <vector>

class Map {
  public:
    void load(const std::filesystem::path &path);

    int height() const;
    int width() const;

    bool contains(int x, int y) const;
    int tile(int x, int y) const;
    bool isWall(int x, int y) const;

  private:
    int height_ = 0;
    int width_ = 0;

    std::vector<int> tiles_;
};