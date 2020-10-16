#ifndef RENDER_CAMERARESOURCE_H_
#define RENDER_CAMERARESOURCE_H_

#include "../math/Math.h"

#include "BaseResource.h"

struct CameraResource : BaseResource {

	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);

};

#endif /* RENDER_CAMERARESOURCE_H_ */
