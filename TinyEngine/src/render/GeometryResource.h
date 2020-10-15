#ifndef RENDER_GEOMETRYRENDER_H_
#define RENDER_GEOMETRYRENDER_H_

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

};

#endif /* RENDER_GEOMETRYRENDER_H_ */
