#ifndef RENDER_LIGHTRENDER_H_
#define RENDER_LIGHTRENDER_H_

#include <string>

#include "BaseResource.h"

struct LightResource : BaseResource {

	std::string environment;

	TextureResource diffuse;
	TextureResource specular;
	TextureResource lut;

};

#endif /* RENDER_LIGHTRENDER_H_ */
