#pragma once

#include <cstdint>
#include <vector>

class Textures {
  public:
    void loadTextures();

    const int texWidth = 64;
    const int texHeight = 64;

    std::vector<uint32_t> texture[8];
  private:
    bool loadImage(std::vector<uint32_t> &pixels, const char *path);
};