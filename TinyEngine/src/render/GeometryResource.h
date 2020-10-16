#ifndef RENDER_GEOMETRYRESOURCE_H_
#define RENDER_GEOMETRYRESOURCE_H_

#include <cstdint>
#include <map>
#include <vector>

#include "../composite/Composite.h"

#include "BaseResource.h"

struct GeometryResource : BaseResource {

	uint32_t count = 0;

	std::map<std::string, std::string> macros;

	std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
	std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
	std::vector<VkBuffer> vertexBuffers;
	std::vector<VkDeviceSize> vertexBuffersOffsets;
	std::vector<VkDeviceSize> vertexBuffersRanges;

	VkPrimitiveTopology primitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

};

#endif /* RENDER_GEOMETRYRESOURCE_H_ */
