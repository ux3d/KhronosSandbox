#ifndef RENDER_HELPERRASTERIZE_H_
#define RENDER_HELPERRASTERIZE_H_

#include <cstdint>

#include "../gltf/GLTF.h"
#include "../resource/ResourceManager.h"

enum DrawMode {
	ALL,
	OPAQUE,
	TRANSPARENT
};

class HelperRasterize {
public:

	static void draw(ResourceManager& resourceManager, const Primitive& primitive, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex, DrawMode drawMode, const glm::mat4& worldMatrix);

	static void draw(ResourceManager& resourceManager, const Mesh& mesh, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex, DrawMode drawMode, const glm::mat4& worldMatrix);

	static void draw(ResourceManager& resourceManager, const Node& node, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex, DrawMode drawMode);

	static void draw(ResourceManager& resourceManager, const Scene& scene, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex, DrawMode drawMode);

	static void draw(ResourceManager& resourceManager, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex, DrawMode drawMode);
};

#endif /* RENDER_HELPERRASTERIZE_H_ */
