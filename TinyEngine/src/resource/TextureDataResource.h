#ifndef RESOURCE_TEXTUREDATARESOURCE_H_
#define RESOURCE_TEXTUREDATARESOURCE_H_

#include "../composite/Composite.h"

#include "BaseResource.h"

struct TextureDataResource : BaseResource {

	TextureResourceCreateInfo textureResourceCreateInfo = {};

	TextureResource textureResource = {};
};

#endif /* RESOURCE_TEXTUREDATARESOURCE_H_ */
