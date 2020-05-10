#ifndef GLTF_HELPERLOOP_H_
#define GLTF_HELPERLOOP_H_

#include "../GLTF.h"

#include "ResourceManager.h"

class HelperLoop {
public:

	static bool update(ResourceManager& resourceManager, Primitive& primitive, GLTF& glTF, const glm::mat4& parentWorldMatrix);
	static void draw(ResourceManager& resourceManager, const Primitive& primitive, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex);

	static bool update(ResourceManager& resourceManager, Mesh& mesh, GLTF& glTF, const glm::mat4& parentWorldMatrix);
	static void draw(ResourceManager& resourceManager, const Mesh& mesh, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex);

	static bool update(ResourceManager& resourceManager, Node& node, GLTF& glTF, const glm::mat4& parentWorldMatrix);
	static void draw(ResourceManager& resourceManager, const Node& node, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex);

	static bool update(ResourceManager& resourceManager, Scene& scene, GLTF& glTF, const glm::mat4& parentWorldMatrix);
	static void draw(ResourceManager& resourceManager, const Scene& scene, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex);

	static bool update(ResourceManager& resourceManager, GLTF& glTF, const glm::mat4& parentWorldMatrix);
	static void draw(ResourceManager& resourceManager, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex);
};

#endif /* GLTF_HELPERLOOP_H_ */
