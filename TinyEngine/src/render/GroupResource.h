#ifndef RENDER_GROUPRESOURCE_H_
#define RENDER_GROUPRESOURCE_H_

#include <cstdint>
#include <vector>

#include "BaseResource.h"

struct GroupResource : BaseResource {

	std::vector<uint64_t> geometryModelHandles;

};

#endif /* RENDER_GROUPRESOURCE_H_ */
