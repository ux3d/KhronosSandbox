#ifndef RESOURCE_CAMERARESOURCE_H_
#define RESOURCE_CAMERARESOURCE_H_

#include "../math/Math.h"

#include "BaseResource.h"

struct CameraResource : BaseResource {

	glm::mat4 projection;
	glm::mat4 view;

};

#endif /* RESOURCE_CAMERARESOURCE_H_ */
