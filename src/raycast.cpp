#include "raycast.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>

constexpr int MAP_WIDTH = 24;
constexpr int MAP_HEIGHT = 24;

int worldMap[MAP_WIDTH][MAP_HEIGHT] = {
    {8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 4, 6, 4, 4, 6, 4, 6, 4, 4, 4, 6},
    {8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
    {8, 0, 3, 3, 0, 0, 0, 0, 0, 8, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6},
    {8, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6},
    {8, 0, 3, 3, 0, 0, 0, 0, 0, 8, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
    {8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 4, 0, 0, 0, 0, 0, 6, 6, 6, 0, 6, 4, 6},
    {8, 8, 8, 8, 0, 8, 8, 8, 8, 8, 8, 4, 4, 4, 4, 4, 4, 6, 0, 0, 0, 0, 0, 6},
    {7, 7, 7, 7, 0, 7, 7, 7, 7, 0, 8, 0, 8, 0, 8, 0, 8, 4, 0, 4, 0, 6, 0, 6},
    {7, 7, 0, 0, 0, 0, 0, 0, 7, 8, 0, 8, 0, 8, 0, 8, 8, 6, 0, 0, 0, 0, 0, 6},
    {7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 6, 0, 0, 0, 0, 0, 4},
    {7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 6, 0, 6, 0, 6, 0, 6},
    {7, 7, 0, 0, 0, 0, 0, 0, 7, 8, 0, 8, 0, 8, 0, 8, 8, 6, 4, 6, 0, 6, 6, 6},
    {7, 7, 7, 7, 0, 7, 7, 7, 7, 8, 8, 4, 0, 6, 8, 4, 8, 3, 3, 3, 0, 3, 3, 3},
    {2, 2, 2, 2, 0, 2, 2, 2, 2, 4, 6, 4, 0, 0, 6, 0, 6, 3, 0, 0, 0, 0, 0, 3},
    {2, 2, 0, 0, 0, 0, 0, 2, 2, 4, 0, 0, 0, 0, 0, 0, 4, 3, 0, 0, 0, 0, 0, 3},
    {2, 0, 0, 0, 0, 0, 0, 0, 2, 4, 0, 0, 0, 0, 0, 0, 4, 3, 0, 0, 0, 0, 0, 3},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 4, 4, 4, 4, 4, 6, 0, 6, 3, 3, 0, 0, 0, 3, 3},
    {2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 1, 2, 2, 2, 6, 6, 0, 0, 5, 0, 5, 0, 5},
    {2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 2, 2, 0, 5, 0, 5, 0, 0, 0, 5, 5},
    {2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 2, 5, 0, 5, 0, 5, 0, 5, 0, 5},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5},
    {2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 2, 5, 0, 5, 0, 5, 0, 5, 0, 5},
    {2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 2, 2, 0, 5, 0, 5, 0, 0, 0, 5, 5},
    {2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 5, 5, 5, 5, 5, 5, 5, 5}};

constexpr float TILE_SIZE = 64.0f;
constexpr float VIEW_DISTANCE = 128.0f;
constexpr float FOV = 75.0f * M_PI / 180.0f;

constexpr float wallHeight = 1.0f;
constexpr float eyeHeight = 0.5f;

Raycast::Raycast(Framebuffer &framebuffer, const Player &player,
                 const Textures &textures)
    : framebuffer_(framebuffer), player_(player), textures_(textures) {}

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

        if (!hit.hit || hit.distance <= 0.0001f)
            continue;

        float brightness =
            std::max(0.15f, 1.0f / (1.0f + hit.distance * 0.15f));

        float projectionScale = width / (2.0f * std::tan(FOV / 2.0f));
        float wallTop = height / 2.0f -
                        (wallHeight - cameraZ) * projectionScale / hit.distance;
        float wallBottom =
            height / 2.0f + cameraZ * projectionScale / hit.distance;

        int drawStart = static_cast<int>(std::ceil(
            std::clamp(wallTop - 0.5f, 0.0f, static_cast<float>(height))));
        int drawEnd = static_cast<int>(std::ceil(
            std::clamp(wallBottom - 0.5f, 0.0f, static_cast<float>(height))));

        float wallX = hit.side == Side::X ? hit.position.y : hit.position.x;
        wallX -= std::floor(wallX);

        int texX = static_cast<int>(wallX * float(textures_.texWidth));
        if (hit.side == Side::X && rayDirection.x > 0)
            texX = textures_.texWidth - texX - 1;
        if (hit.side == Side::Y && rayDirection.y < 0)
            texX = textures_.texWidth - texX - 1;

        float shade = shadingEnabled_
                          ? brightness * (hit.side == Side::Y ? 0.5f : 1.0f)
                          : 1.0f;
        for (int y = drawStart; y < drawEnd; y++) {
            float v = (y + 0.5f - wallTop) / (wallBottom - wallTop);
            int texY = std::clamp(static_cast<int>(v * textures_.texHeight), 0,
                                  textures_.texHeight - 1);
            std::uint32_t color = 0xFF808080u;
            if (texturesEnabled_) {
                color = textures_.texture[hit.texNum]
                                         [textures_.texWidth * texY + texX];
            }
            auto r = static_cast<std::uint32_t>((color & 0xFFu) * shade);
            auto g = static_cast<std::uint32_t>(((color >> 8) & 0xFFu) * shade);
            auto b =
                static_cast<std::uint32_t>(((color >> 16) & 0xFFu) * shade);
            pixels[y * width + x] = 0xFF000000u | (b << 16) | (g << 8) | r;
        }
    }
}

void Raycast::toggleTextures() { texturesEnabled_ = !texturesEnabled_; }

void Raycast::toggleShading() { shadingEnabled_ = !shadingEnabled_; }

Hit Raycast::castRay(Vec2 start, Vec2 direction) {
    Cell mapPos = {static_cast<int>(std::floor(start.x)),
                   static_cast<int>(std::floor(start.y))};
    if (mapPos.x < 0 || mapPos.x >= MAP_WIDTH || mapPos.y < 0 ||
        mapPos.y >= MAP_HEIGHT || (direction.x == 0.0f && direction.y == 0.0f))
        return {};
    Cell step;

    Vec2 deltaDist = {std::abs(1 / direction.x), std::abs(1 / direction.y)};
    Vec2 sideDist;

    bool hit = false;
    Side side = Side::X;

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
        if (distance > VIEW_DISTANCE || mapPos.x < 0 || mapPos.x >= MAP_WIDTH ||
            mapPos.y < 0 || mapPos.y >= MAP_HEIGHT)
            return {};
        if (worldMap[mapPos.y][mapPos.x] > 0)
            hit = true;
    }
    Vec2 position = {start.x + distance * direction.x,
                     start.y + distance * direction.y};
    return {hit, distance, side, position, worldMap[mapPos.y][mapPos.x] - 1};
}
