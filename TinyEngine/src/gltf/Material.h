#ifndef GLTF_MATERIAL_H_
#define GLTF_MATERIAL_H_

#include <cstdint>

#include "../math/Math.h"

#include "PbrMetallicRoughness.h"

struct Material {
	PbrMetallicRoughness pbrMetallicRoughness;

	NormalTextureInfo normalTexture;

	OcclusionTextureInfo occlusionTexture;

	TextureInfo emissiveTexture;
	glm::vec3 emissiveFactor = glm::vec3(0.0f);

	uint32_t alphaMode = 0;

	float alphaCutoff = 0.5f;

	bool doubleSided = false;
};

#endif /* GLTF_MATERIAL_H_ */
