#pragma once

#include <SDL3/SDL.h>
#include <optional>

#include "framebuffer.h"
#include "map.h"
#include "player.h"
#include "raycast.h"
#include "renderer.h"
#include "textures.h"

class Game {
  public:
    ~Game();

    bool init();
    void run();

  private:
    void processEvents();
    void update(float deltaTime);
    void render();
    void shutdown();

    SDL_Window *window_ = nullptr;
    SDL_GPUDevice *device_ = nullptr;
    bool sdlInitialized_ = false;
    bool windowClaimed_ = false;

    Framebuffer framebuffer_{640, 360};
    Renderer renderer_;
    Textures textures_;
    Player player_;
    Map map_;
    std::optional<Raycast> raycast_;

    bool running_ = false;
    bool VsyncEnabled_ = true;
    Uint64 previousCounter_ = 0;
};
