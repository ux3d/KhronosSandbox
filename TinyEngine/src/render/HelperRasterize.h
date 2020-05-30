#ifndef RENDER_HELPERRASTERIZE_H_
#define RENDER_HELPERRASTERIZE_H_

#include <cstdint>

#include "../gltf/GLTF.h"

#include "../render/AllocationManager.h"

enum DrawMode {
	ALL,
	OPAQUE,
	TRANSPARENT
};

class HelperRasterize {
public:

	static void draw(AllocationManager& allocationManager, const Primitive& primitive, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex, DrawMode drawMode, const glm::mat4& worldMatrix);

	static void draw(AllocationManager& allocationManager, const Mesh& mesh, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex, DrawMode drawMode, const glm::mat4& worldMatrix);

	static void draw(AllocationManager& allocationManager, const Node& node, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex, DrawMode drawMode);

	static void draw(AllocationManager& allocationManager, const Scene& scene, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex, DrawMode drawMode);

	static void draw(AllocationManager& allocationManager, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex, DrawMode drawMode);
};

#endif /* RENDER_HELPERRASTERIZE_H_ */
