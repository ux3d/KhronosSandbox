#ifndef GLTF_TEXTUREINFO_H_
#define GLTF_TEXTUREINFO_H_

#include <cstdint>

struct TextureInfo {
	int32_t 	index = -1;
	uint32_t	texCoord = 0;
};

struct NormalTextureInfo : public TextureInfo {
	float scale = 1.0f;
};

struct OcclusionTextureInfo : public TextureInfo {
	float strength = 1.0f;
};

#endif /* GLTF_TEXTUREINFO_H_ */
