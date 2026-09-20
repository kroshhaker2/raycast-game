cmake_minimum_required(VERSION 3.25)

project(RayFPS
    VERSION 0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL3 REQUIRED CONFIG)

add_executable(rayfps
    src/main.cpp
    src/framebuffer.cpp
    src/renderer.cpp
    src/raycast.cpp
    src/player.cpp
)

target_link_libraries(rayfps PRIVATE
    SDL3::SDL3
)
