#include "player.h"

#include <cmath>

void Player::handleKeyDown(SDL_Scancode key) {
    switch (key) {
    case SDL_SCANCODE_W:
        forward_ = true;
        break;

    case SDL_SCANCODE_S:
        backward_ = true;
        break;

    case SDL_SCANCODE_A:
        left_ = true;
        break;

    case SDL_SCANCODE_D:
        right_ = true;
        break;

    default:
        break;
    }
}

void Player::handleKeyUp(SDL_Scancode key) {
    switch (key) {
    case SDL_SCANCODE_W:
        forward_ = false;
        break;

    case SDL_SCANCODE_S:
        backward_ = false;
        break;

    case SDL_SCANCODE_A:
        left_ = false;
        break;

    case SDL_SCANCODE_D:
        right_ = false;
        break;

    default:
        break;
    }
}

void Player::update(float deltaTime) {
    if (left_) {
        yaw_ -= rotationSpeed_ * deltaTime;
    }

    if (right_) {
        yaw_ += rotationSpeed_ * deltaTime;
    }

    float directionX = std::cos(yaw_);
    float directionY = std::sin(yaw_);

    float movement = 0.0f;

    if (forward_) {
        movement += moveSpeed_ * deltaTime;
    }

    if (backward_) {
        movement -= moveSpeed_ * deltaTime;
    }

    x_ += directionX * movement;
    y_ += directionY * movement;
}

float Player::x() const { return x_; }

float Player::y() const { return y_; }

float Player::yaw() const { return yaw_; }