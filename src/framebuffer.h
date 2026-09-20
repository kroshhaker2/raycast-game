#pragma once

#include <cstdint>
#include <vector>

class Framebuffer {
  public:
    Framebuffer(int width, int height);

    void clear(std::uint32_t color);

    void setPixel(int x, int y, std::uint32_t color);
    std::uint32_t getPixel(int x, int y) const;

    int width() const;
    int height() const;

    std::uint32_t *data();
    const std::uint32_t *data() const;

  private:
    int width_;
    int height_;

    std::vector<std::uint32_t> pixels_;
};