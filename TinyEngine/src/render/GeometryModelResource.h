#ifndef RENDER_GEOMETRYMODELRESOURCE_H_
#define RENDER_GEOMETRYMODELRESOURCE_H_

#include <cstdint>
#include <string>
#include <map>

#include "../composite/Composite.h"

#include "BaseResource.h"

struct GeometryModelResource : BaseResource {

	uint64_t geometryHandle = 0;

	uint64_t materialHandle = 0;

	VkIndexType indexType = VK_INDEX_TYPE_NONE_KHR;
	VkBuffer indexBuffer = VK_NULL_HANDLE;
	uint32_t indexOffset = 0;
	uint32_t indexRange = 0;

	uint32_t indicesCount = 0;
	uint32_t verticesCount = 0;

	std::map<std::string, std::string> macros;

	VkCullModeFlags cullMode = VK_CULL_MODE_NONE;

	uint32_t targetsCount = 0;

	uint64_t targetPositionHandle = 0;
	uint64_t targetNormalHandle = 0;
	uint64_t targetTangentHandle = 0;

	uint64_t indexHandle = 0;

	uint32_t mode = 4;
	VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

};

#endif /* RENDER_GEOMETRYMODELRESOURCE_H_ */
