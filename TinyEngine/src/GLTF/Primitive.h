#ifndef GLTF_PRIMITIVE_H_
#define GLTF_PRIMITIVE_H_

#include "../TinyEngine.h"

class GLTF;

class Primitive {
public:
	int32_t position = -1;
	int32_t normal = -1;
	int32_t tangent = -1;
	int32_t texCoord0 = -1;
	int32_t texCoord1 = -1;
	int32_t color0 = -1;
	int32_t joints0 = -1;
	int32_t joints1 = -1;
	int32_t weights0 = -1;
	int32_t weights1 = -1;

	int32_t indices = -1;

	int32_t material = -1;

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

	glm::mat4 worldMatrix;

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

#endif /* GLTF_PRIMITIVE_H_ */
