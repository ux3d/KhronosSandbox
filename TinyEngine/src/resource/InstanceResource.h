#ifndef RESOURCE_INSTANCERESOURCE_H_
#define RESOURCE_INSTANCERESOURCE_H_

#include <cstdint>

#include "../math/Math.h"

struct InstanceResource {

	// Mapper helper

	uint64_t groupHandle;

	glm::mat4 worldMatrix;

};

#endif /* RESOURCE_INSTANCERESOURCE_H_ */
