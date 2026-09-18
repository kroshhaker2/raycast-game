{
  description = "C++ Vulkan renderer development environment";

  inputs.nixpkgs.url = "https://flakehub.com/f/NixOS/nixpkgs/0";

  outputs =
    { self, ... }@inputs:

    let
      supportedSystems = [
        "x86_64-linux"
        "aarch64-linux"
        "aarch64-darwin"
      ];

      forEachSupportedSystem =
        f:
        inputs.nixpkgs.lib.genAttrs supportedSystems (
          system:
          f {
            inherit system;
            pkgs = import inputs.nixpkgs { inherit system; };
          }
        );
    in
    {
      devShells = forEachSupportedSystem (
        { pkgs, ... }:
        {
          default = pkgs.mkShell {
            packages = with pkgs; [
              # C++
              clang
              clang-tools
              cmake
              gdb
              cppcheck
              gtest

              # Vulkan
              vulkan-headers
              vulkan-loader
              vulkan-tools
              vulkan-validation-layers

              # Shaders
              shaderc
              glslang

              # Window
              glfw
              mangohud

              # GPU memory
              vulkan-memory-allocator

              # Math
              glm

              # Utilities
              pkg-config
              git

              # Formatting
              nixfmt
            ];

            shellHook = ''
              export VULKAN_SDK="${pkgs.vulkan-validation-layers}"
            '';
          };
        }
      );

      formatter = forEachSupportedSystem (
        { pkgs, ... }:
        pkgs.nixfmt
      );
    };
}
