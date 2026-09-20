#pragma once

#include <SDL3/SDL_scancode.h>

class Player {
  public:
    void handleKeyDown(SDL_Scancode key);
    void handleKeyUp(SDL_Scancode key);

    void update(float deltaTime);

    float x() const;
    float y() const;
    float yaw() const;

  private:
    bool forward_ = false;
    bool backward_ = false;
    bool left_ = false;
    bool right_ = false;

    float x_ = 2.5f;
    float y_ = 2.5f;
    float yaw_ = 0.0f;

    float moveSpeed_ = 3.0f;
    float rotationSpeed_ = 2.5f;
};
