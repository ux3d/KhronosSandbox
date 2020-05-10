#ifndef GLTF_SCENE_H_
#define GLTF_SCENE_H_

#include "../TinyEngine.h"

class GLTF;

struct InstanceResource {
	glm::mat4 worldMatrix;
	int32_t materialIndex = -1;
	int32_t componentTypeSize = -1;

	int32_t normalInstanceID = -1;
	int32_t tangentInstanceID = -1;
	int32_t texCoord0InstanceID = -1;

	//

	int32_t padding[3];
};

class Scene {
public:
	std::vector<int32_t> nodes;
};

#endif /* GLTF_SCENE_H_ */
