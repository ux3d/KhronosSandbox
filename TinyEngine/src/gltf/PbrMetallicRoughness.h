#ifndef GLTF_PBRMETALLICROUGHNESS_H_
#define GLTF_PBRMETALLICROUGHNESS_H_

#include "../math/Math.h"

#include "TextureInfo.h"

struct PbrMetallicRoughness {
	glm::vec4 baseColorFactor = glm::vec4(1.0f);
	TextureInfo baseColorTexture;

	float metallicFactor = 1.0f;
	float roughnessFactor = 1.0f;

	TextureInfo metallicRoughnessTexture;
};

#endif /* GLTF_PBRMETALLICROUGHNESS_H_ */
