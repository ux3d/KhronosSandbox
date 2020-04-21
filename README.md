# glTF 2.0 raytrace vs. pathtrace vs. rasterize using Vulkan

This R&D project is a playground to evaluate raytracing vs. pathtracing vs. rasterizing glTF 2.0 with it's Metallic-Roughness material model.  

![Screenshot DamagedHelmet](screenshot.jpg)

Evaluation
- [x] glTF rasterize
- [x] glTF raytrace
- [x] glTF pathtrace
- [ ] Pre-filtered vs. spherical harmonics

## How to build?
- [Vulkan SDK 1.2.135.0 or later](https://vulkan.lunarg.com/sdk/home)
- [NVIDIA Vulkan 1.2 Developer Beta Driver or later](https://developer.nvidia.com/vulkan-driver)
   - NVIDIA GPU capable for raytracing
- [Eclipse C/C++ Development Tooling](https://projects.eclipse.org/projects/tools.cdt)
   - GCC for Linux
   - [MinGW64](https://www.msys2.org/) for Windows 

1. Import the projects into Eclipse.
2. Select `Windows` or `Linux` build - either as `Release` or `Debug` build.
3. Build the `TinyEngine` project first.
4. Build the `Application` project.
5. Working directory is `Application` both for `Release` and `Debug` build.

## Tested platforms
- Windows 10 (64bit)
- Ubuntu 19.10 (64bit)

## Used tools
- [Eclipse C/C++ Development Tooling](https://projects.eclipse.org/projects/tools.cdt)
- [Gestaltor - A glTF editor](https://gestaltor.io/)
- [MSYS2](https://www.msys2.org/) on Windows  
  Execute `pacman -S mingw-w64-x86_64-gcc` to install MinGW64

## Used assets
- [glTF 2.0 Sample Models](https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0)
- [glTF Sample Environments](https://github.com/ux3d/glTF-Sample-Environments)

## Used libraries
- [glTF Sample Viewer](https://github.com/KhronosGroup/glTF-Sample-Viewer)
- [GLFW](https://github.com/glfw/glfw)
- [glm](https://github.com/g-truc/glm)  
- [Slim KTX2](https://github.com/ux3d/slimktx2)  
- [tiny glTF](https://github.com/syoyo/tinygltf)  
   - [json](https://github.com/nlohmann/json)
   - [stb](https://github.com/nothings/stb)  
- [volk](https://github.com/zeux/volk)  
- [Vulkan SDK](https://vulkan.lunarg.com/)
   - [Shaderc](https://github.com/google/shaderc)

## Limitiations by purpose
- Vulkan  
  - No resize/maximize of window possible. This avoids resource destroying/creating and simplifies the Vulkan code.
  - During minimize, the update loop is stopped.
  - Helper methods for initalization are not optimal and causing a stall on the GPU. Again, this is done for Vulkan code simplicity.
- Tooling
  - gcc
  - C++17
  - 64bit

## References
- [NVIDIA DesignWorks Samples](https://github.com/nvpro-samples/)
- [NVIDIA Vulkan Ray Tracing Tutorial](https://nvpro-samples.github.io/vk_raytracing_tutorial_KHR/)
- [Sascha's Vulkan Samples Raytracing](https://github.com/SaschaWillems/Vulkan-Samples/tree/VK_KHR_ray_tracing)
