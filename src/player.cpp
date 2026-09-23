#include "player.h"

#include <cmath>
#include <numbers>

void Player::handleKeyDown(SDL_Scancode key) {
    switch (key) {
    case SDL_SCANCODE_W:
        forward_ = true;
        break;

    case SDL_SCANCODE_S:
        backward_ = true;
        break;

    case SDL_SCANCODE_A:
        strafeLeft_ = true;
        break;

    case SDL_SCANCODE_D:
        strafeRight_ = true;
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
        strafeLeft_ = false;
        break;

    case SDL_SCANCODE_D:
        strafeRight_ = false;
        break;

    default:
        break;
    }
}

void Player::handleMouseMotion(float deltaX) {
    yaw_ += deltaX * mouseSensitivity_;

    yaw_ = std::remainder(yaw_, 2.0f * std::numbers::pi_v<float>);
}

void Player::update(float deltaTime) {
    float directionX = std::cos(yaw_);
    float directionY = std::sin(yaw_);

    float rightYaw = yaw_ + std::numbers::pi_v<float> / 2.0f;
    float leftYaw = yaw_ - std::numbers::pi_v<float> / 2.0f;

    float rightX = -directionY;
    float rightY = directionX;

    float forwardInput = float(forward_) - float(backward_);
    float strafeInput = float(strafeRight_) - float(strafeLeft_);

    float movement = step(forwardInput, moveSpeed_, deltaTime);
    float strafeMovement = step(strafeInput, strafeSpeed_, deltaTime);

    x_ += directionX * movement;
    y_ += directionY * movement;

    x_ += rightX * strafeMovement;
    y_ += rightY * strafeMovement;
}

float Player::step(float input, float &speed, float deltaTime) {
    float targetSpeed = input * maxMoveSpeed_;

    float response = input == 0.0f ? 25.0f : 20.0f;
    float blend = 1.0f - std::exp(-response * deltaTime);

    speed += (targetSpeed - speed) * blend;

    if (std::abs(targetSpeed - speed) < 0.001f) {
        speed = targetSpeed;
    }

    return speed * deltaTime;
}

float Player::x() const { return x_; }

float Player::y() const { return y_; }

float Player::yaw() const { return yaw_; }