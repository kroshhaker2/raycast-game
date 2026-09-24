#pragma once

#include <SDL3/SDL_scancode.h>

class Player {
  public:
    void handleKeyDown(SDL_Scancode key);
    void handleKeyUp(SDL_Scancode key);

    void handleMouseMotion(float deltaX);

    void update(float deltaTime);

    float x() const;
    float y() const;
    float z() const;
    float yaw() const;

  private:
    bool forward_ = false;
    bool backward_ = false;
    bool strafeLeft_ = false;
    bool strafeRight_ = false;
    bool space_ = false;

    float x_ = 2.5f;
    float y_ = 2.5f;
    float z_ = 0.0f;
    float yaw_ = 0.0f;

    float maxMoveSpeed_ = 3.0f;
    float moveSpeed_ = 0.0f;

    float strafeSpeed_ = 2.5f;

    float verticalSpeed_;

    float rotationSpeed_ = 2.5f;

    float mouseSensitivity_ = 0.0025f;

    float step(float input, float &speed, float deltaTime);
    float gravity(float input, float &speed, float gravityAcceleration,
                  float deltaTime);
};
