#ifndef RENDER_CAMERARENDER_H_
#define RENDER_CAMERARENDER_H_

#include "../math/Math.h"

#include "BaseResource.h"

struct CameraResource : BaseResource {

	glm::mat4 projection;
	glm::mat4 view;

};

#endif /* RENDER_CAMERARENDER_H_ */
