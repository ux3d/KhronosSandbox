#ifndef GLTF_SCENE_H_
#define GLTF_SCENE_H_

#include "../TinyEngine.h"

class GLTF;

struct InstanceResource {
	glm::mat4 worldMatrix;
	int32_t materialIndex = -1;
	int32_t componentTypeSize = -1;

	int32_t normalInstanceID = -1;
	int32_t tangentInstanceID = -1;
	int32_t texCoord0InstanceID = -1;

	//

	int32_t padding[3];
};

class Scene {
public:
	std::vector<int32_t> nodes;

	// Rasterize helper

	// Raytrace helper

	std::vector<VkAccelerationStructureInstanceKHR> accelerationStructureInstances;
	BufferResource accelerationStructureInstanceBuffer = {};
	TopLevelResource topLevelResource = {};

	std::vector<InstanceResource> instanceResources;
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

#endif /* GLTF_SCENE_H_ */
