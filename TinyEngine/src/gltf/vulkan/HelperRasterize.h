#ifndef GLTF_HELPERRASTERIZE_H_
#define GLTF_HELPERRASTERIZE_H_

#include "../GLTF.h"

#include "ResourceManager.h"

class HelperRasterize {
public:

	static void draw(ResourceManager& resourceManager, const Primitive& primitive, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex);

	static void draw(ResourceManager& resourceManager, const Mesh& mesh, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex);

	static void draw(ResourceManager& resourceManager, const Node& node, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex);

	static void draw(ResourceManager& resourceManager, const Scene& scene, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex);

	static void draw(ResourceManager& resourceManager, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex);
};

#endif /* GLTF_HELPERRASTERIZE_H_ */
