#ifndef GLTF_HELPERRAYTRACE_H_
#define GLTF_HELPERRAYTRACE_H_

#include "../GLTF.h"

#include "ResourceManager.h"

class HelperRaytrace {
public:

	static void draw(ResourceManager& resourceManager, const Scene& scene, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex, uint32_t width, uint32_t height);

	static void draw(ResourceManager& resourceManager, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex, uint32_t width, uint32_t height);

};

#endif /* GLTF_VULKAN_HELPERRAYTRACE_H_ */
