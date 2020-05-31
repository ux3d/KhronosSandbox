#ifndef RESOURCE_LIGHTRESOURCE_H_
#define RESOURCE_LIGHTRESOURCE_H_

#include <string>

#include "BaseResource.h"

struct LightResource : BaseResource {

	std::string environment;

	TextureResource diffuse;
	TextureResource specular;
	TextureResource lut;

};

#endif /* RESOURCE_LIGHTRESOURCE_H_ */
