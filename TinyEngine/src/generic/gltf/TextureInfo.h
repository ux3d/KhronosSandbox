#ifndef GLTF_TEXTUREINFO_H_
#define GLTF_TEXTUREINFO_H_

#include "../../TinyEngine.h"

class TextureInfo {
public:
	int32_t 	index = -1;
	uint32_t	texCoord = 0;
};

class NormalTextureInfo : public TextureInfo {
public:
	float scale = 1.0f;
};

class OcclusionTextureInfo : public TextureInfo {
public:
	float strength = 1.0f;
};

#endif /* GLTF_TEXTUREINFO_H_ */
