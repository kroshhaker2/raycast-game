#pragma once

#include "framebuffer.h"
#include "player.h"

enum class Side { X, Y };

struct Vec3 {
    float x;
    float y;
    float z;
};

struct Vec2 {
    float x;
    float y;
};

struct Cell {
    int x;
    int y;
};

struct Hit {
    bool hit;
    float distance;
    Side side;
    Vec2 position;
};

class Raycast {
  public:
    Raycast(Framebuffer &framebuffer, const Player &player);

    void renderFrame();

    // explicit Raycast(const Map& map);

  private:
    Framebuffer &framebuffer_;
    const Player &player_;
    // const Map &map_;

    std::vector<uint32_t> texture[8];

    Hit castRay(Vec2 start, Vec2 direction);
};