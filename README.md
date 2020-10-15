# Sandbox for Vulkan, SPIR-V, glTF and OpenXR

This [R&D](https://en.wikipedia.org/wiki/Research_and_development) project is a sandbox to evaluate [Khronos](https://www.khronos.org/) API's like [Vulkan](https://www.khronos.org/vulkan/), [SPIR-V](https://www.khronos.org/spir/), [glTF](https://www.khronos.org/gltf/) and [OpenXR](https://www.khronos.org/openxr/).

![Screenshot DamagedHelmet](screenshot.jpg)

## TinyEngine architecture

![Components](Architecture/components.png)

## Work in Progress

### Vulkan  
- Done

### SPIR-V
- Done

### glTF  
- [ ] Store resources per node
- [ ] Skinning

### OpenXR  
- [ ] Initial component

### General  
- [ ] Replace slimktx2 with libktx

## How to build?
- [Vulkan SDK 1.2.135.0 or later](https://vulkan.lunarg.com/sdk/home)
- [Eclipse C/C++ Development Tooling](https://projects.eclipse.org/projects/tools.cdt)
   - GCC for Linux
   - [MinGW64](https://www.msys2.org/) for Windows 

### Build steps for Eclipse

1. Import the projects into Eclipse.
2. Select `Windows` or `Linux` build - either as `Release` or `Debug` build.
3. Build the `TinyEngine` project first.
4. Build the `ExampleXX` project.
5. Working directory is `ExampleXX` both for `Release` and `Debug` build.

## Tested platforms
- Windows 10 (64bit)
- Ubuntu 20.04 LTS (64bit)

## Used tools
- [Eclipse C/C++ Development Tooling](https://projects.eclipse.org/projects/tools.cdt)
- [Gestaltor - Visual glTF editor](https://gestaltor.io/)
- [MSYS2](https://www.msys2.org/) on Windows  
  Execute `pacman -S mingw-w64-x86_64-gcc` to install MinGW64  
  Execute `pacman -S mingw-w64-x86_64-gdb` to install gdb for MinGW64  

## Used assets
- [glTF 2.0 Sample Models](https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0)
- [glTF Sample Environments](https://github.com/ux3d/glTF-Sample-Environments)

## Used libraries
- [basis_universal](https://github.com/BinomialLLC/basis_universal)
- [glTF Sample Viewer](https://github.com/KhronosGroup/glTF-Sample-Viewer)
- [GLFW](https://github.com/glfw/glfw)
- [glm](https://github.com/g-truc/glm)
- [imgui](https://github.com/ocornut/imgui)
- [Shaderc](https://github.com/google/shaderc)  
- [Slim KTX2](https://github.com/ux3d/slimktx2)
   - basis_universal
- [stb](https://github.com/nothings/stb)    
- [tiny glTF](https://github.com/syoyo/tinygltf)  
- [volk](https://github.com/zeux/volk)  
- [Vulkan SDK](https://vulkan.lunarg.com/)
   - Shaderc

## Limitiations by purpose
- Vulkan  
  - During minimize, the update loop is stopped.
  - Helper methods for initalization are not optimal and causing a stall on the GPU. This is done for Vulkan code simplicity.
- Tooling
  - gcc (Windows and Linux)
  - C++17
  - 64bit only
