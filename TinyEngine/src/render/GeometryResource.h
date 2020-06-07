#ifndef RENDER_GEOMETRYRENDER_H_
#define RENDER_GEOMETRYRENDER_H_

#include <cstdint>
#include <map>
#include <vector>

#include "../composite/Composite.h"

#include "BaseResource.h"

struct GeometryResource : BaseResource {
	// Mapper helper

	uint32_t count = 0;

	std::map<std::string, std::string> macros;

	// Rasterize helper

	std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
	std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
	std::vector<VkBuffer> vertexBuffers;
	std::vector<VkDeviceSize> vertexBuffersOffsets;
	std::vector<VkDeviceSize> vertexBuffersRanges;

	// Raytrace helper

	int32_t positionAttributeIndex = -1;
	int32_t normalAttributeIndex = -1;
	int32_t tangentAttributeIndex = -1;
	int32_t texCoord0AttributeIndex = -1;
	int32_t texCoord1AttributeIndex = -1;
	int32_t color0AttributeIndex = -1;
	int32_t joints0AttributeIndex = -1;
	int32_t joints1AttributeIndex = -1;
	int32_t weights0AttributeIndex = -1;
	int32_t weights1AttributeIndex = -1;
};

#endif /* RENDER_GEOMETRYRENDER_H_ */
