#ifndef GLTF_PRIMITIVE_H_
#define GLTF_PRIMITIVE_H_

#include <cstdint>
#include <vector>

#include "../math/Math.h"

struct Target {
	int32_t position = -1;
	int32_t normal = -1;
	int32_t tangent = -1;
};

struct Primitive {
	int32_t position = -1;
	int32_t normal = -1;
	int32_t tangent = -1;
	int32_t texCoord0 = -1;
	int32_t texCoord1 = -1;
	int32_t color0 = -1;
	int32_t joints0 = -1;
	int32_t joints1 = -1;
	int32_t weights0 = -1;
	int32_t weights1 = -1;

	int32_t indices = -1;

	int32_t material = -1;

	VkPrimitiveTopology mode = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	std::vector<Target> targets;

	// Generic Helper

	uint32_t attributesCount = 0;

	std::vector<glm::vec3> targetPositionData;
	std::vector<glm::vec3> targetNormalData;
	std::vector<glm::vec3> targetTangentData;
};

#endif /* GLTF_PRIMITIVE_H_ */
