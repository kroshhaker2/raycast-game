#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_timer.h>

#include "framebuffer.h"
#include "player.h"
#include "raycast.h"
#include "renderer.h"

int main(int argc, char *argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window =
        SDL_CreateWindow("Game", 1280, 720, SDL_WINDOW_RESIZABLE);

    if (!window) {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_GPUDevice *device =
        SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);

    if (!device) {
        SDL_Log("SDL_CreateGPUDevice failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    if (!SDL_ClaimWindowForGPUDevice(device, window)) {
        SDL_Log("SDL_ClaimWindowForGPUDevice failed: %s", SDL_GetError());

        SDL_DestroyGPUDevice(device);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool running = true;
    SDL_Event event;

    Framebuffer fb(640, 360);
    Renderer renderer;
    Player player;

    if (!renderer.init(device, window, fb.width(), fb.height())) {
        SDL_Log("Renderer initialization failed");

        SDL_DestroyGPUDevice(device);
        SDL_DestroyWindow(window);
        SDL_Quit();

        return 1;
    }

    Raycast raycast(fb, player);

    Uint64 previous = SDL_GetPerformanceCounter();

    while (running) {
        Uint64 current = SDL_GetPerformanceCounter();

        float deltaTime = static_cast<float>(current - previous) /
                          SDL_GetPerformanceFrequency();

        previous = current;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_QUIT:
                running = false;
                break;

            case SDL_EVENT_KEY_DOWN:
                player.handleKeyDown(event.key.scancode);
                if (event.key.scancode == SDL_SCANCODE_ESCAPE) {
                    if (!SDL_SetWindowRelativeMouseMode(window, false)) {
                        SDL_Log("Mouse release failed: %s", SDL_GetError());
                    }
                } else {
                    player.handleKeyDown(event.key.scancode);
                }
                break;

            case SDL_EVENT_KEY_UP:
                player.handleKeyUp(event.key.scancode);
                break;

            case SDL_EVENT_MOUSE_MOTION:
                if (SDL_GetWindowRelativeMouseMode(window)) {
                    player.handleMouseMotion(event.motion.xrel);
                }
                break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (!SDL_SetWindowRelativeMouseMode(window, true)) {
                        SDL_Log("Mouse capture failed: %s", SDL_GetError());
                    }
                }
                break;

            default:
                break;
            }
        }

        player.update(deltaTime);

        fb.clear(0xFF'00'00'00);

        raycast.renderFrame();

        renderer.draw(window, fb);
    }

    renderer.shutdown();

    SDL_ReleaseWindowFromGPUDevice(device, window);
    SDL_DestroyGPUDevice(device);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}