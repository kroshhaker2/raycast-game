#include "game.h"

#include <exception>

Game::~Game() { shutdown(); }

bool Game::init() {
    if (sdlInitialized_ || window_ || device_) {
        SDL_Log("Game is already initialized");
        return false;
    }

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return false;
    }
    sdlInitialized_ = true;

    window_ = SDL_CreateWindow("Game", 1280, 720, SDL_WINDOW_RESIZABLE);

    if (!window_) {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        shutdown();
        return false;
    }

    device_ = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);

    if (!device_) {
        SDL_Log("SDL_CreateGPUDevice failed: %s", SDL_GetError());
        shutdown();
        return false;
    }

    if (!SDL_ClaimWindowForGPUDevice(device_, window_)) {
        SDL_Log("SDL_ClaimWindowForGPUDevice failed: %s", SDL_GetError());
        shutdown();
        return false;
    }
    windowClaimed_ = true;

    SDL_GPUPresentMode presentMode = SDL_GPU_PRESENTMODE_VSYNC;
    if (SDL_WindowSupportsGPUPresentMode(device_, window_,
                                         SDL_GPU_PRESENTMODE_MAILBOX)) {
        presentMode = SDL_GPU_PRESENTMODE_MAILBOX;
    } else if (SDL_WindowSupportsGPUPresentMode(
                   device_, window_, SDL_GPU_PRESENTMODE_IMMEDIATE)) {
        presentMode = SDL_GPU_PRESENTMODE_IMMEDIATE;
    }

    if (presentMode == SDL_GPU_PRESENTMODE_VSYNC) {
        SDL_Log("Only VSync is supported; disabling the FPS cap may still "
                "leave a display refresh limit");
    } else if (!SDL_SetGPUSwapchainParameters(device_, window_,
                                              SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
                                              presentMode)) {
        SDL_Log("Cannot disable VSync: %s", SDL_GetError());
        presentMode = SDL_GPU_PRESENTMODE_VSYNC;
    }

    VsyncEnabled_ = presentMode == SDL_GPU_PRESENTMODE_VSYNC;

    try {
        map_.load("assets/maps/level1.map");
        textures_.loadTextures();
    } catch (const std::exception &error) {
        SDL_Log("Game resources initialization failed: %s", error.what());
        shutdown();
        return false;
    }

    if (!renderer_.init(device_, window_, framebuffer_.width(),
                        framebuffer_.height())) {
        SDL_Log("Renderer initialization failed");
        shutdown();
        return false;
    }

    raycast_.emplace(framebuffer_, player_, textures_, map_);
    running_ = true;

    return true;
}

void Game::run() {
    previousCounter_ = SDL_GetPerformanceCounter();

    while (running_) {
        processEvents();

        Uint64 current = SDL_GetPerformanceCounter();

        float deltaTime = static_cast<float>(current - previousCounter_) /
                          static_cast<float>(SDL_GetPerformanceFrequency());

        previousCounter_ = current;

        update(deltaTime);
        render();
    }
}

void Game::update(float deltaTime) { player_.update(deltaTime); }

void Game::render() {
    framebuffer_.clear(0xFF'00'00'00);

    if (raycast_) {
        raycast_->renderFrame();
    }

    renderer_.draw(window_, framebuffer_);
}

void Game::processEvents() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_EVENT_QUIT:
            running_ = false;
            break;

        case SDL_EVENT_KEY_DOWN:
            if (event.key.scancode == SDL_SCANCODE_F2) {
                if (!event.key.repeat) {
                    raycast_->toggleTextures();
                }
            } else if (event.key.scancode == SDL_SCANCODE_F3) {
                if (!event.key.repeat) {
                    raycast_->toggleShading();
                }
            } else if (event.key.scancode == SDL_SCANCODE_F4) {
                if (!event.key.repeat) {
                    VsyncEnabled_ = !VsyncEnabled_;
                }
            } else if (event.key.scancode == SDL_SCANCODE_F11) {
                if (!event.key.repeat) {
                    bool fullscreen = (SDL_GetWindowFlags(window_) &
                                       SDL_WINDOW_FULLSCREEN) != 0;
                    if (!SDL_SetWindowFullscreen(window_, !fullscreen)) {
                        SDL_Log("Fullscreen toggle failed: %s", SDL_GetError());
                    }
                }
            } else if (event.key.scancode == SDL_SCANCODE_ESCAPE) {
                if (!SDL_SetWindowRelativeMouseMode(window_, false)) {
                    SDL_Log("Mouse release failed: %s", SDL_GetError());
                }
            } else {
                player_.handleKeyDown(event.key.scancode);
            }
            break;

        case SDL_EVENT_KEY_UP:
            player_.handleKeyUp(event.key.scancode);
            break;

        case SDL_EVENT_MOUSE_MOTION:
            if (SDL_GetWindowRelativeMouseMode(window_)) {
                player_.handleMouseMotion(event.motion.xrel);
            }
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (event.button.button == SDL_BUTTON_LEFT) {
                if (!SDL_SetWindowRelativeMouseMode(window_, true)) {
                    SDL_Log("Mouse capture failed: %s", SDL_GetError());
                }
            }
            break;

        default:
            break;
        }
    }
}

void Game::shutdown() {
    running_ = false;
    raycast_.reset();

    renderer_.shutdown();

    if (windowClaimed_) {
        SDL_ReleaseWindowFromGPUDevice(device_, window_);
        windowClaimed_ = false;
    }

    if (device_) {
        SDL_DestroyGPUDevice(device_);
        device_ = nullptr;
    }

    if (window_) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }

    if (sdlInitialized_) {
        SDL_Quit();
        sdlInitialized_ = false;
    }
}
