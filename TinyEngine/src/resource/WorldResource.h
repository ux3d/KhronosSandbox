#ifndef RESOURCE_WORLDRESOURCE_H_
#define RESOURCE_WORLDRESOURCE_H_

#include <cstdint>
#include <vector>

#include "../composite/Composite.h"
#include "../math/Math.h"

#include "BaseResource.h"

struct ViewProjectionUniformPushConstant {
	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
};

struct ModelViewProjectionUniformPushConstant {
	ViewProjectionUniformPushConstant viewProjection = {};
	glm::mat4 world = glm::mat4(1.0f);
};

struct InverseViewProjectionUniformPushConstant {
	glm::mat4 inverseProjection;
	glm::mat4 inverseView;
};

struct RaytraceUniformPushConstant {
	InverseViewProjectionUniformPushConstant inverseViewProjection = {};
	uint32_t maxDepth = 0;
	uint32_t specularSamples = 1;
	uint32_t diffuseSamples = 1;
};


struct RaytracePrimitiveUniformBuffer {
	glm::mat4 worldMatrix;
	int32_t materialIndex = -1;
	int32_t componentTypeSize = -1;

	int32_t normalInstanceID = -1;
	int32_t tangentInstanceID = -1;
	int32_t texCoord0InstanceID = -1;

	//

	int32_t padding[3];
};

struct WorldResource : BaseResource {
	// Mapper helper

	std::vector<uint64_t> instanceHandles;

	uint64_t lightHandle;

	// Rasterize helper

	ViewProjectionUniformPushConstant viewProjection = {};

	TextureResource diffuse;
	TextureResource specular;
	TextureResource lut;

	// Raytrace helper

	RaytraceUniformPushConstant raytrace = {};

	std::vector<VkAccelerationStructureInstanceKHR> accelerationStructureInstances;
	BufferResource accelerationStructureInstanceBuffer = {};
	TopLevelResource topLevelResource = {};

	std::vector<RaytracePrimitiveUniformBuffer> instanceResources;
	StorageBufferResource instanceResourcesStorageBufferResource;

	StorageBufferResource materialStorageBufferResource;

	VkDescriptorSetLayout raytraceDescriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorPool raytraceDescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSet raytraceDescriptorSet = VK_NULL_HANDLE;

	VkPipelineLayout raytracePipelineLayout = VK_NULL_HANDLE;

	VkShaderModule rayGenShaderModule = VK_NULL_HANDLE;
	VkShaderModule missShaderModule = VK_NULL_HANDLE;
	VkShaderModule closestHitShaderModule = VK_NULL_HANDLE;

	VkPipeline raytracePipeline = VK_NULL_HANDLE;

	BufferResource shaderBindingBufferResource = {};
	VkDeviceSize size = 0;
};

#endif /* RESOURCE_WORLDRESOURCE_H_ */
