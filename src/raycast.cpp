#include "raycast.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>

int map[5][5] = {{1, 1, 1, 1, 1},
                 {1, 0, 0, 0, 1},
                 {1, 0, 1, 0, 1},
                 {1, 0, 0, 0, 1},
                 {1, 1, 1, 1, 1}};

constexpr float TILE_SIZE = 64.0f;
constexpr float VIEW_DISTANCE = 128.0f;
constexpr float FOV = 60.0f * M_PI / 180.0f;

Raycast::Raycast(Framebuffer &framebuffer) : framebuffer_(framebuffer) {}

void Raycast::renderFrame() {
    Vec2 player = {1.0f, 1.0f};
    float yaw = 45.0f;

    float yawRad = yaw * M_PI / 180.0f;

    Vec2 cameraDirection = {std::cos(yawRad), std::sin(yawRad)};
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

        Hit hit = castRay(player, rayDirection);

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

        int wallTop = height / 2 - lineHeight / 2;
        int wallBottom = height / 2 + lineHeight / 2;

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
        if (map[mapPos.y][mapPos.x] > 0)
            hit = true;
    }
    return {hit, distance, side, sideDist};
}