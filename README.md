# Sandbox for Vulkan, SPIR-V and glTF

This [R&D](https://en.wikipedia.org/wiki/Research_and_development) project is a sandbox to evaluate [Khronos](https://www.khronos.org/) API's like [Vulkan](https://www.khronos.org/vulkan/), [SPIR-V](https://www.khronos.org/spir/) and [glTF](https://www.khronos.org/gltf/).  

![Screenshot DamagedHelmet](screenshot.jpg)

## TinyEngine architecture

![Components](Architecture/components.png)

## Work in Progress

### Vulkan  
- Done

### SPIR-V
- Done

### glTF  
- Done

### General  
- [ ] Prefilter IBL on the fly
- [ ] Remove Slim KTX2 dependency
- [ ] Remove BasisU dependency
- [ ] Remove TinyGLTF dependency

## How to build?
- [Vulkan SDK 1.3.231.1 or later](https://vulkan.lunarg.com/sdk/home)
  - Earlier versions do work with high chance but are not tested
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
- [MSYS2](https://www.msys2.org/) on Windows  
  `pacman -S mingw-w64-x86_64-gcc`  
  `pacman -S mingw-w64-x86_64-gdb`  

## Used assets
- [glTF 2.0 Sample Models](https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0)
- [glTF Sample Environments](https://github.com/ux3d/glTF-Sample-Environments)

## Used libraries and code
- [GLFW](https://github.com/glfw/glfw)  
  `pacman -S mingw-w64-x86_64-glfw`
- [glm](https://github.com/g-truc/glm)  
  `pacman -S mingw-w64-x86_64-glm`
- [OpenImageIO](https://github.com/OpenImageIO/oiio)  
  `pacman -S mingw-w64-x86_64-openimageio`
- [Shaderc](https://github.com/google/shaderc)  
  `pacman -S mingw-w64-x86_64-shaderc`
- [Slim KTX2](https://github.com/ux3d/slimktx2)  
   - basis_universal
- [tiny glTF](https://github.com/syoyo/tinygltf)  
- [volk](https://github.com/zeux/volk)  
   - Integrated in TinyEngine
- [Vulkan SDK](https://vulkan.lunarg.com/)  

## Limitiations by purpose
- Vulkan  
  - During minimize, the update loop is stopped.
  - Helper methods for initalization are not optimal and causing a stall on the GPU. This is done for Vulkan code simplicity.
- Tooling
  - gcc (Windows and Linux)
  - C++17
  - 64bit only
