# RayFPS

Небольшой raycasting-проект на C++23 и SDL3.

## Сборка

Нужны CMake 3.25+, Clang с поддержкой C++23 и SDL3.
Для окружения через Nix: `nix develop`.

```bash
cmake -S . -B build -DCMAKE_CXX_COMPILER=clang++
cmake --build build
```

## Запуск

```bash
./build/rayfps
```
