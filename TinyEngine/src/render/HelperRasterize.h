#ifndef RENDER_HELPERRASTERIZE_H_
#define RENDER_HELPERRASTERIZE_H_

#include <cstdint>

#include "../gltf/GLTF.h"

#include "ResourceManager.h"

class HelperRasterize {
public:

	static void draw(ResourceManager& resourceManager, const Primitive& primitive, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex, const glm::mat4& worldMatrix);

	static void draw(ResourceManager& resourceManager, const Mesh& mesh, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex, const glm::mat4& worldMatrix);

	static void draw(ResourceManager& resourceManager, const Node& node, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex);

	static void draw(ResourceManager& resourceManager, const Scene& scene, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex);

	static void draw(ResourceManager& resourceManager, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex);
};

#endif /* RENDER_HELPERRASTERIZE_H_ */
