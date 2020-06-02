#ifndef RENDER_GEOMETRYMODELRENDER_H_
#define RENDER_GEOMETRYMODELRENDER_H_

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

	uint32_t indicesCount = 0;
	uint32_t verticesCount = 0;

	// Rasterize helper

	VkShaderModule vertexShaderModule = VK_NULL_HANDLE;
	VkShaderModule fragmentShaderModule = VK_NULL_HANDLE;

	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkPipeline graphicsPipeline = VK_NULL_HANDLE;

	StorageBufferResource* targetPosition = nullptr;
	StorageBufferResource* targetNormal = nullptr;
	StorageBufferResource* targetTangent = nullptr;

	// Raytrace helper

	BottomLevelResource bottomLevelResource = {};
};

#endif /* RENDER_GEOMETRYMODELRENDER_H_ */
