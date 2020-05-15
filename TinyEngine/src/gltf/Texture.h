#ifndef GLTF_TEXTURE_H_
#define GLTF_TEXTURE_H_

#include <cstdint>

struct Texture {
	int32_t source = -1;
	int32_t sampler = -1;
};

#endif /* GLTF_TEXTURE_H_ */
