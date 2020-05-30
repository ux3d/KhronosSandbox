#ifndef RESOURCE_WORLDRESOURCE_H_
#define RESOURCE_WORLDRESOURCE_H_

#include <cstdint>

#include "../composite/Composite.h"
#include "../math/Math.h"

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

struct MaterialUniformBuffer {
	glm::vec4 baseColorFactor = glm::vec4(1.0f);

	float metallicFactor = 1.0f;
	float roughnessFactor = 1.0f;
	float normalScale = 1.0f;
	float occlusionStrength = 1.0f;

	glm::vec3 emissiveFactor = glm::vec3(0.0f);
	uint32_t alphaMode = 0;

	float alphaCutoff = 0.5f;
	bool doubleSided = false;
};

struct RaytraceMaterialUniformBuffer {
	MaterialUniformBuffer materialUniformBuffer;

	int32_t baseColorTexture = -1;
	int32_t metallicRoughnessTexture = -1;

	int32_t emissiveTexture = -1;

	int32_t occlusionTexture = -1;

	int32_t normalTexture = -1;

	//

	int32_t padding;
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

struct WorldResource {
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
