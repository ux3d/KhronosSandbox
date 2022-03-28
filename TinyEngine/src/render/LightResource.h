#ifndef RENDER_LIGHTRESOURCE_H_
#define RENDER_LIGHTRESOURCE_H_

#include <string>

#include "../composite/Composite.h"

#include "BaseResource.h"

struct LightResource : BaseResource {

	std::string environment;

	TextureResource lambertian;
	TextureResource ggx;
	TextureResource ggxLUT;

};

#endif /* RENDER_LIGHTRESOURCE_H_ */
