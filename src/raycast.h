#pragma once

#include "framebuffer.h"
#include "player.h"
#include "textures.h"

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
    int texNum;
};

class Raycast {
  public:
    Raycast(Framebuffer &framebuffer, const Player &player,
            const Textures &textures);

    void renderFrame();

    void toggleTextures();
    void toggleShading();

    // explicit Raycast(const Map& map);

  private:
    Framebuffer &framebuffer_;
    const Player &player_;
    const Textures &textures_;
    bool texturesEnabled_ = true;
    bool shadingEnabled_ = true;
    // const Map &map_;

    Hit castRay(Vec2 start, Vec2 direction);
};
