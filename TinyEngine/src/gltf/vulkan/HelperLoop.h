#ifndef GLTF_HELPERLOOP_H_
#define GLTF_HELPERLOOP_H_

#include "../GLTF.h"

class HelperLoop {
public:

	static bool update(Primitive& primitive, GLTF& glTF, const glm::mat4& parentWorldMatrix);
	static void draw(const Primitive& primitive, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex);

	static bool update(Mesh& mesh, GLTF& glTF, const glm::mat4& parentWorldMatrix);
	static void draw(const Mesh& mesh, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex);

	static bool update(Node& node, GLTF& glTF, const glm::mat4& parentWorldMatrix);
	static void draw(const Node& node, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex);

	static bool update(Scene& scene, GLTF& glTF, const glm::mat4& parentWorldMatrix);
	static void draw(const Scene& scene, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex);

	static bool update(GLTF& glTF, const glm::mat4& parentWorldMatrix);
	static void draw(const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex);
};

#endif /* GLTF_HELPERLOOP_H_ */
