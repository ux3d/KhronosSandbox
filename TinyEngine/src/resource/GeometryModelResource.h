#ifndef RESOURCE_GEOMETRYMODELRESOURCE_H_
#define RESOURCE_GEOMETRYMODELRESOURCE_H_

#include <cstdint>
#include <vector>

#include "../composite/Composite.h"

struct GeometryModelResource {
	// Mapper helper

	uint64_t materialHandle = 0;

	VkIndexType indexType = VK_INDEX_TYPE_NONE_KHR;
	VkBuffer indexBuffer = VK_NULL_HANDLE;
	uint32_t indexOffset = 0;

	uint32_t count = 0;

	// Rasterize helper

	std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
	std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
	std::vector<VkBuffer> vertexBuffers;
	std::vector<VkDeviceSize> vertexBuffersOffsets;

	VkShaderModule vertexShaderModule = VK_NULL_HANDLE;
	VkShaderModule fragmentShaderModule = VK_NULL_HANDLE;

	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkPipeline graphicsPipeline = VK_NULL_HANDLE;

	StorageBufferResource targetPosition;
	StorageBufferResource targetNormal;
	StorageBufferResource targetTangent;

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

#endif /* RESOURCE_GEOMETRYMODELRESOURCE_H_ */
