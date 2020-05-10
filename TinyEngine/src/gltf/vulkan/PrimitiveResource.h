#ifndef GLTF_PRIMITIVERESOURCE_H_
#define GLTF_PRIMITIVERESOURCE_H_

#include "../../TinyEngine.h"

class PrimitiveResource {
public:
	// Rasterize helper

	uint32_t attributesCount = 0;
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

	int32_t positionLocation = -1;
	int32_t normalLocation = -1;
	int32_t tangentLocation = -1;
	int32_t texCoord0Location = -1;
	int32_t texCoord1Location = -1;
	int32_t color0Location = -1;
	int32_t joints0Location = -1;
	int32_t joints1Location = -1;
	int32_t weights0Location = -1;
	int32_t weights1Location = -1;
};

#endif /* GLTF_PRIMITIVERESOURCE_H_ */
