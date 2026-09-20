#pragma once

#include "framebuffer.h"

enum class Side {
  X,
  Y
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
    Raycast(Framebuffer &framebuffer);

    void renderFrame();

    // explicit Raycast(const Map& map);

  private:
    Framebuffer &framebuffer_;
    // const Map &map_;

    Hit castRay(Vec2 start, Vec2 direction);
};