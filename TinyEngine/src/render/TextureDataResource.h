#ifndef RENDER_TEXTUREDATARENDER_H_
#define RENDER_TEXTUREDATARENDER_H_

#include "../composite/Composite.h"

#include "BaseResource.h"

struct TextureDataResource : BaseResource {

	TextureResourceCreateInfo textureResourceCreateInfo = {};

	TextureResource textureResource = {};

	uint32_t textureIndex = 0;
};

#endif /* RENDER_TEXTUREDATARENDER_H_ */
