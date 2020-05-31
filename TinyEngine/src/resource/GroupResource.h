#ifndef RESOURCE_GROUPRESOURCE_H_
#define RESOURCE_GROUPRESOURCE_H_

#include <cstdint>
#include <vector>

#include "BaseResource.h"

struct GroupResource : BaseResource {

	// Mapper helper

	std::vector<uint64_t> geometryModelHandles;

};

#endif /* RESOURCE_GROUPRESOURCE_H_ */
