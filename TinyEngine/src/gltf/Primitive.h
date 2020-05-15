#ifndef GLTF_PRIMITIVE_H_
#define GLTF_PRIMITIVE_H_

#include <cstdint>

#include "../math/Math.h"

class Primitive {
public:
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

	// Generic Helper

	glm::mat4 worldMatrix;

	uint32_t attributesCount = 0;
};

#endif /* GLTF_PRIMITIVE_H_ */
