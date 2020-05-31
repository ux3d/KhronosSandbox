#ifndef RESOURCE_GEOMETRYMODELRESOURCE_H_
#define RESOURCE_GEOMETRYMODELRESOURCE_H_

#include <cstdint>
#include <vector>

#include "../composite/Composite.h"

#include "BaseResource.h"

struct GeometryModelResource : BaseResource {
	// Mapper helper

	uint64_t geometryHandle = 0;

	uint64_t materialHandle = 0;

	VkIndexType indexType = VK_INDEX_TYPE_NONE_KHR;
	VkBuffer indexBuffer = VK_NULL_HANDLE;
	uint32_t indexOffset = 0;

	uint32_t count = 0;

	// Rasterize helper

	VkShaderModule vertexShaderModule = VK_NULL_HANDLE;
	VkShaderModule fragmentShaderModule = VK_NULL_HANDLE;

	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkPipeline graphicsPipeline = VK_NULL_HANDLE;

	StorageBufferResource targetPosition;
	StorageBufferResource targetNormal;
	StorageBufferResource targetTangent;

	// Raytrace helper

	BottomLevelResource bottomLevelResource = {};
};

#endif /* RESOURCE_GEOMETRYMODELRESOURCE_H_ */
