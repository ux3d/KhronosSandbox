#ifndef RENDER_PRIMITIVERESOURCE_H_
#define RENDER_PRIMITIVERESOURCE_H_

#include <cstdint>
#include <vector>

#include "../composite/Composite.h"

class PrimitiveResource {
public:
	// Rasterize helper

	std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
	std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
	std::vector<VkBuffer> vertexBuffers;
	std::vector<VkDeviceSize> vertexBuffersOffsets;

	VkShaderModule vertexShaderModule = VK_NULL_HANDLE;
	VkShaderModule fragmentShaderModule = VK_NULL_HANDLE;

	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkPipeline graphicsPipeline = VK_NULL_HANDLE;

	// Raytrace helper

	BottomLevelResource bottomLevelResource = {};

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

#endif /* RENDER_PRIMITIVERESOURCE_H_ */
