#ifndef RESOURCE_GEOMETRYRESOURCE_H_
#define RESOURCE_GEOMETRYRESOURCE_H_

#include <cstdint>
#include <vector>

#include "../composite/Composite.h"

struct GeometryResource {
	// Mapper helper

	uint32_t attributeIndex = 0;

	// Rasterize helper

	std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
	std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
	std::vector<VkBuffer> vertexBuffers;
	std::vector<VkDeviceSize> vertexBuffersOffsets;

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

#endif /* RESOURCE_GEOMETRYRESOURCE_H_ */
