#ifndef RENDER_GROUPRENDER_H_
#define RENDER_GROUPRENDER_H_

#include <cstdint>
#include <vector>

#include "BaseResource.h"

struct GroupResource : BaseResource {

	std::vector<uint64_t> geometryModelHandles;

};

#endif /* RENDER_GROUPRENDER_H_ */
