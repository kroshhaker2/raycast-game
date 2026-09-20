#pragma once

#include <SDL3/SDL.h>

#include "framebuffer.h"

class Renderer {
  public:
    bool init(SDL_GPUDevice *device, SDL_Window *window, int width, int height);

    void draw(SDL_Window *window, const Framebuffer &framebuffer);
    void shutdown();

  private:
    SDL_GPUDevice *device_ = nullptr;
    SDL_GPUTexture *texture_ = nullptr;
    SDL_GPUTransferBuffer *transfer_buffer_ = nullptr;

    std::size_t buffer_size_ = 0;
};