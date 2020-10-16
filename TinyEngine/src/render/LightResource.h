#ifndef RENDER_LIGHTRESOURCE_H_
#define RENDER_LIGHTRESOURCE_H_

#include <string>

#include "../composite/Composite.h"

#include "BaseResource.h"

struct LightResource : BaseResource {

	std::string environment;

	TextureResource diffuse;
	TextureResource specular;
	TextureResource lut;

};

#endif /* RENDER_LIGHTRESOURCE_H_ */
