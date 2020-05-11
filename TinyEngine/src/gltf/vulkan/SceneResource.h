#ifndef GLTF_SCENERESOURCE_H_
#define GLTF_SCENERESOURCE_H_

#include "../../TinyEngine.h"

#include "GltfResource.h"

class SceneResource {
public:
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
};

#endif /* GLTF_SCENERESOURCE_H_ */
