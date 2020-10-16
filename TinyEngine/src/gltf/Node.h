#ifndef GLTF_NODE_H_
#define GLTF_NODE_H_

#include <cstdint>
#include <vector>

#include "../math/Math.h"

struct Node {
	glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::mat4 matrix = glm::mat4(1.0f);

	std::vector<int32_t> children;

	int32_t mesh = -1;

	int32_t skin = -1;

	std::vector<float> weights;

	// Generic helper

	glm::mat4 worldMatrix;
};

#endif /* GLTF_NODE_H_ */
