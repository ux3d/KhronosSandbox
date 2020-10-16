#ifndef GLTF_SKIN_H_
#define GLTF_SKIN_H_

#include <cstdint>
#include <vector>

struct Skin {
	int32_t inverseBindMatrices = -1;

	int32_t skeleton = -1;

	std::vector<int32_t> joints;
};

#endif /* GLTF_SKIN_H_ */
