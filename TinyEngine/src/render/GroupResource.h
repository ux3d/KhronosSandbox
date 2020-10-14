#ifndef RENDER_GROUPRENDER_H_
#define RENDER_GROUPRENDER_H_

#include <cstdint>
#include <vector>

#include "BaseResource.h"

struct GroupResource : BaseResource {

	// Mapper helper

	std::vector<uint64_t> geometryModelHandles;

	// Rasterize helper

	uint64_t weightsHandle = 0;

};

#endif /* RENDER_GROUPRENDER_H_ */
