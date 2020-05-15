#ifndef GLTF_NODE_H_
#define GLTF_NODE_H_

#include <vector>

#include "../math/Math.h"

class GLTF;

class Node {
public:
	glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::mat4 matrix = glm::mat4(1.0f);

	std::vector<int32_t> children;

	int32_t mesh = -1;

	// Generic helper

	glm::mat4 worldMatrix;
};

#endif /* GLTF_NODE_H_ */
