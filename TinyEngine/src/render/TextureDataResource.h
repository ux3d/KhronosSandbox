#ifndef RENDER_TEXTUREDATARENDER_H_
#define RENDER_TEXTUREDATARENDER_H_

#include "../composite/Composite.h"

#include "BaseResource.h"

struct TextureDataResource : BaseResource {

	TextureResourceCreateInfo textureResourceCreateInfo = {};

	TextureResource textureResource = {};

	int32_t textureIndex = -1;
};

#endif /* RENDER_TEXTUREDATARENDER_H_ */
