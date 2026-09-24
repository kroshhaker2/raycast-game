#include "raycast.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>

#define texWidth 64
#define texHeight 64

constexpr int MAP_WIDTH = 31;
constexpr int MAP_HEIGHT = 25;

int worldMap[MAP_WIDTH][MAP_HEIGHT] = {
    {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7, 7, 7, 7, 7, 7, 7, 7},
    {4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 7},
    {4, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7},
    {4, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7},
    {4, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 7},
    {4, 0, 4, 0, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 7, 7, 0, 7, 7, 7, 7, 7},
    {4, 0, 5, 0, 0, 0, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 7, 0, 0, 0, 7, 7, 7, 1},
    {4, 0, 6, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 7, 0, 0, 0, 0, 0, 0, 8},
    {4, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 1},
    {4, 0, 8, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 7, 0, 0, 0, 0, 0, 0, 8},
    {4, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 7, 0, 0, 0, 7, 7, 7, 1},
    {4, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 5, 5, 5, 5, 7, 7, 7, 7, 7, 7, 7, 1},
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
    {8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
    {6, 6, 6, 6, 6, 6, 0, 6, 6, 6, 6, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
    {4, 4, 4, 4, 4, 4, 0, 4, 4, 4, 6, 0, 6, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3},
    {4, 0, 0, 0, 0, 0, 0, 0, 0, 4, 6, 0, 6, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2},
    {4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 2, 0, 0, 5, 0, 0, 2, 0, 0, 0, 2},
    {4, 0, 0, 0, 0, 0, 0, 0, 0, 4, 6, 0, 6, 2, 0, 0, 0, 0, 0, 2, 2, 0, 2, 2},
    {4, 0, 6, 0, 6, 0, 0, 0, 0, 4, 6, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 2},
    {4, 0, 0, 5, 0, 0, 0, 0, 0, 4, 6, 0, 6, 2, 0, 0, 0, 0, 0, 2, 2, 0, 2, 2},
    {4, 0, 6, 0, 6, 0, 0, 0, 0, 4, 6, 0, 6, 2, 0, 0, 5, 0, 0, 2, 0, 0, 0, 2},
    {4, 0, 0, 0, 0, 0, 0, 0, 0, 4, 6, 0, 6, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2},
    {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3}};

constexpr float TILE_SIZE = 64.0f;
constexpr float VIEW_DISTANCE = 128.0f;
constexpr float FOV = 75.0f * M_PI / 180.0f;

constexpr float wallHeight = 1.0f;
constexpr float eyeHeight = 0.5f;

Raycast::Raycast(Framebuffer &framebuffer, const Player &player)
    : framebuffer_(framebuffer), player_(player) {
    for (auto &tex : texture) {
        tex.resize(texWidth * texHeight);
    }
    // generate some textures
    for (int x = 0; x < texWidth; x++)
        for (int y = 0; y < texHeight; y++) {
            int xorcolor = (x * 256 / texWidth) ^ (y * 256 / texHeight);
            // int xcolor = x * 256 / texWidth;
            int ycolor = y * 256 / texHeight;
            int xycolor = y * 128 / texHeight + x * 128 / texWidth;
            texture[0][texWidth * y + x] =
                65536 * 254 *
                (x != y &&
                 x != texWidth - y); // flat red texture with black cross
            texture[1][texWidth * y + x] =
                xycolor + 256 * xycolor + 65536 * xycolor; // sloped greyscale
            texture[2][texWidth * y + x] =
                256 * xycolor + 65536 * xycolor; // sloped yellow gradient
            texture[3][texWidth * y + x] =
                xorcolor + 256 * xorcolor + 65536 * xorcolor; // xor greyscale
            texture[4][texWidth * y + x] = 256 * xorcolor;    // xor green
            texture[5][texWidth * y + x] =
                65536 * 192 * (x % 16 && y % 16);          // red bricks
            texture[6][texWidth * y + x] = 65536 * ycolor; // red gradient
            texture[7][texWidth * y + x] =
                128 + 256 * 128 + 65536 * 128; // flat grey texture
        }
}

void Raycast::renderFrame() {
    Vec3 player = {player_.x(), player_.y(), player_.z()};
    float yaw = player_.yaw();
    float cameraZ = player.z + eyeHeight;

    Vec2 cameraDirection = {std::cos(yaw), std::sin(yaw)};
    Vec2 cameraPlane = {-cameraDirection.y * std::tan(FOV / 2),
                        cameraDirection.x * std::tan(FOV / 2)};

    uint32_t *pixels = framebuffer_.data();
    framebuffer_.clear(0xFF000000);

    int width = framebuffer_.width();
    int height = framebuffer_.height();

    for (int x = 0; x < width; x++) {
        float cameraX = 2.0f * x / (width - 1) - 1.0f;

        Vec2 rayDirection = {cameraDirection.x + cameraPlane.x * cameraX,
                             cameraDirection.y + cameraPlane.y * cameraX};

        Hit hit = castRay({player.x, player.y}, rayDirection);

        if (!hit.hit)
            continue;

        float brightness =
            std::max(0.15f, 1.0f / (1.0f + hit.distance * 0.15f));

        std::uint32_t color = 0xFF'80'80'80;

        std::uint8_t r = (color >> 0) & 0xFF;
        std::uint8_t g = (color >> 8) & 0xFF;
        std::uint8_t b = (color >> 16) & 0xFF;

        r *= brightness;
        g *= brightness;
        b *= brightness;

        std::uint32_t shaded = (0xFFu << 24) |
                               (static_cast<std::uint32_t>(b) << 16) |
                               (static_cast<std::uint32_t>(g) << 8) |
                               static_cast<std::uint32_t>(r);

        int lineHeight = static_cast<int>(height / hit.distance);

        float projectionScale = width / (2.0f * std::tan(FOV / 2.0f));

        float wallTop = height / 2.0f -
                        (wallHeight - player_.z()) * projectionScale / hit.distance;

        float wallBottom =
            height / 2.0f + cameraZ * projectionScale / hit.distance;

        if (wallTop < 0)
            wallTop = 0;

        if (wallBottom >= height)
            wallBottom = height - 1;

        for (int y = wallTop; y < wallBottom; y++) {
            pixels[y * width + x] = shaded;
        }
    }
}

Hit Raycast::castRay(Vec2 start, Vec2 direction) {
    Cell mapPos = {static_cast<int>(start.x), static_cast<int>(start.y)};
    Cell step;

    Vec2 deltaDist = {std::abs(1 / direction.x), std::abs(1 / direction.y)};
    Vec2 sideDist;

    bool hit = false;
    Side side;

    if (direction.x < 0) {
        step.x = -1;
        sideDist.x = (start.x - mapPos.x) * deltaDist.x;
    } else {
        step.x = 1;
        sideDist.x = (mapPos.x + 1.0f - start.x) * deltaDist.x;
    }

    if (direction.y < 0) {
        step.y = -1;
        sideDist.y = (start.y - mapPos.y) * deltaDist.y;
    } else {
        step.y = 1;
        sideDist.y = (mapPos.y + 1.0f - start.y) * deltaDist.y;
    }

    float distance = 0.0f;

    while (!hit) {
        if (sideDist.x < sideDist.y) {
            sideDist.x += deltaDist.x;
            mapPos.x += step.x;
            side = Side::X;

            distance = sideDist.x - deltaDist.x;
        } else {
            sideDist.y += deltaDist.y;
            mapPos.y += step.y;
            side = Side::Y;

            distance = sideDist.y - deltaDist.y;
        }
        if (distance > VIEW_DISTANCE)
            break;
        if (worldMap[mapPos.y][mapPos.x] > 0)
            hit = true;
    }
    return {hit, distance, side, sideDist};
}