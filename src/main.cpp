#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "framebuffer.h"
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

    if (!renderer.init(device, window, fb.width(), fb.height())) {
        SDL_Log("Renderer initialization failed");

        SDL_DestroyGPUDevice(device);
        SDL_DestroyWindow(window);
        SDL_Quit();

        return 1;
    }

    Raycast raycast(fb);

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

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