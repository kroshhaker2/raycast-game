#include "framebuffer.h"

Framebuffer::Framebuffer(int width, int height)
    : width_(width), height_(height),
      pixels_(static_cast<std::size_t>(width) * height) {}

void Framebuffer::clear(std::uint32_t color) {
    std::fill(pixels_.begin(), pixels_.end(), color);
}

void Framebuffer::setPixel(int x, int y, std::uint32_t color) {
    if (x < 0 || x >= width_ || y < 0 || y >= height_) {
        return;
    }

    pixels_[static_cast<std::size_t>(y) * width_ + x] = color;
}

std::uint32_t Framebuffer::getPixel(int x, int y) const {
    if (x < 0 || x >= width_ || y < 0 || y >= height_) {
        return 0;
    }

    return pixels_[static_cast<std::size_t>(y) * width_ + x];
}

int Framebuffer::width() const { return width_; }

int Framebuffer::height() const { return height_; }

std::uint32_t *Framebuffer::data() { return pixels_.data(); }

const std::uint32_t *Framebuffer::data() const { return pixels_.data(); }