#ifndef GLTF_SKIN_H_
#define GLTF_SKIN_H_

#include <cstdint>
#include <vector>

#include "../math/Math.h"

struct Skin {
	std::vector<glm::mat4> inverseBindMatrices;

	int32_t skeleton = -1;

	std::vector<int32_t> joints;
};

#endif /* GLTF_SKIN_H_ */
