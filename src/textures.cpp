#include "textures.h"

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_surface.h>

void Textures::loadTextures() {
    loadImage(texture[0], "assets/textures/eagle.png");
    loadImage(texture[1], "assets/textures/redbrick.png");
    loadImage(texture[2], "assets/textures/purplestone.png");
    loadImage(texture[3], "assets/textures/greystone.png");
    loadImage(texture[4], "assets/textures/bluestone.png");
    loadImage(texture[5], "assets/textures/mossy.png");
    loadImage(texture[6], "assets/textures/wood.png");
    loadImage(texture[7], "assets/textures/colorstone.png");
}

bool Textures::loadImage(std::vector<uint32_t> &pixels, const char *path) {
    SDL_Surface *source = SDL_LoadPNG(path);
    if (!source) {
        SDL_Log("Cannot load %s: %s", path, SDL_GetError());
        return false;
    }

    SDL_Surface *image = SDL_ConvertSurface(source, SDL_PIXELFORMAT_ABGR8888);
    SDL_DestroySurface(source);

    if (!image) {
        SDL_Log("Cannot convert %s: %s", path, SDL_GetError());
        return false;
    }

    if (image->w != texWidth || image->h != texHeight) {
        SDL_Log("Texture %s must be %dx%d, got %dx%d", path, texWidth,
                texHeight, image->w, image->h);
        SDL_DestroySurface(image);
        return false;
    }

    pixels.resize(texWidth * texHeight);

    const auto *data = static_cast<const std::uint8_t *>(image->pixels);

    for (int y = 0; y < texHeight; ++y) {
        memcpy(pixels.data() + y * texWidth, data + y * image->pitch,
                    texWidth * sizeof(std::uint32_t));
    }

    SDL_DestroySurface(image);
    return true;
}