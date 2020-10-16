#ifndef RENDER_TEXTUREDATARESOURCE_H_
#define RENDER_TEXTUREDATARESOURCE_H_

#include <cstdint>

#include "../composite/Composite.h"

#include "BaseResource.h"

struct TextureDataResource : BaseResource {

	TextureResourceCreateInfo textureResourceCreateInfo = {};

	TextureResource textureResource = {};

	int32_t textureIndex = -1;

};

#endif /* RENDER_TEXTUREDATARESOURCE_H_ */
