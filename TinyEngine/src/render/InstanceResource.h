#ifndef RENDER_INSTANCERENDER_H_
#define RENDER_INSTANCERENDER_H_

#include <cstdint>

#include "../math/Math.h"

#include "BaseResource.h"

struct InstanceResource : BaseResource {

	// Mapper helper

	uint64_t groupHandle;

	glm::mat4 worldMatrix = glm::mat4(1.0f);

};

#endif /* RENDER_INSTANCERENDER_H_ */
