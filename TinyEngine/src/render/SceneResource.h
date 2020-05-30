#ifndef RENDER_SCENERESOURCE_H_
#define RENDER_SCENERESOURCE_H_

#include <vector>

#include "../composite/Composite.h"

#include "WorldResource.h"

struct SceneResource {
	// Raytrace helper

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

#endif /* RENDER_SCENERESOURCE_H_ */
