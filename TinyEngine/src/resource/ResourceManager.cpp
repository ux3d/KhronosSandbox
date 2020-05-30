#include "ResourceManager.h"

void ResourceManager::terminate(BufferViewResource& bufferViewResource, VkDevice device)
{
	VulkanResource::destroyVertexBufferResource(device, bufferViewResource.vertexBufferResource);
}

void ResourceManager::terminate(TextureResource& textureResource, VkDevice device)
{
	VulkanResource::destroyTextureResource(device, textureResource);
}

void ResourceManager::terminate(MaterialResource& materialResource, VkDevice device)
{
	// Descriptor sets do not have to be freed, as managed by pool.
	materialResource.descriptorSet = VK_NULL_HANDLE;

	if (materialResource.descriptorPool != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorPool(device, materialResource.descriptorPool, nullptr);
		materialResource.descriptorPool = VK_NULL_HANDLE;
	}

	if (materialResource.descriptorSetLayout != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorSetLayout(device, materialResource.descriptorSetLayout, nullptr);
		materialResource.descriptorSetLayout = VK_NULL_HANDLE;
	}

	VulkanResource::destroyUniformBufferResource(device, materialResource.uniformBufferResource);
}

void ResourceManager::terminate(PrimitiveResource& primitiveResource, VkDevice device)
{
	if (primitiveResource.graphicsPipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(device, primitiveResource.graphicsPipeline, nullptr);
		primitiveResource.graphicsPipeline = VK_NULL_HANDLE;
	}

	if (primitiveResource.pipelineLayout != VK_NULL_HANDLE)
	{
		vkDestroyPipelineLayout(device, primitiveResource.pipelineLayout, nullptr);
		primitiveResource.pipelineLayout = VK_NULL_HANDLE;
	}

	if (primitiveResource.vertexShaderModule != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(device, primitiveResource.vertexShaderModule, nullptr);
		primitiveResource.vertexShaderModule = VK_NULL_HANDLE;
	}

	if (primitiveResource.fragmentShaderModule != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(device, primitiveResource.fragmentShaderModule, nullptr);
		primitiveResource.fragmentShaderModule = VK_NULL_HANDLE;
	}

	//

	VulkanResource::destroyStorageBufferResource(device, primitiveResource.targetPosition);
	VulkanResource::destroyStorageBufferResource(device, primitiveResource.targetNormal);
	VulkanResource::destroyStorageBufferResource(device, primitiveResource.targetTangent);

	//

	VulkanRaytraceResource::destroyBottomLevelResource(device, primitiveResource.bottomLevelResource);
}

void ResourceManager::terminate(GroupResource& groupResource, VkDevice device)
{
}

void ResourceManager::terminate(InstanceResource& instanceResource, VkDevice device)
{
}

void ResourceManager::terminate(SceneResource& sceneResource, VkDevice device)
{
	VulkanResource::destroyStorageBufferResource(device, sceneResource.instanceResourcesStorageBufferResource);
	VulkanResource::destroyStorageBufferResource(device, sceneResource.materialStorageBufferResource);

	VulkanRaytraceResource::destroyTopLevelResource(device, sceneResource.topLevelResource);
	VulkanResource::destroyBufferResource(device, sceneResource.accelerationStructureInstanceBuffer);

	VulkanResource::destroyBufferResource(device, sceneResource.shaderBindingBufferResource);
	sceneResource.size = 0;

	sceneResource.accelerationStructureInstances.clear();

	if (sceneResource.raytracePipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(device, sceneResource.raytracePipeline, nullptr);
		sceneResource.raytracePipeline = VK_NULL_HANDLE;
	}

	if (sceneResource.rayGenShaderModule != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(device, sceneResource.rayGenShaderModule, nullptr);
		sceneResource.rayGenShaderModule = VK_NULL_HANDLE;
	}

	if (sceneResource.missShaderModule != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(device, sceneResource.missShaderModule, nullptr);
		sceneResource.missShaderModule = VK_NULL_HANDLE;
	}

	if (sceneResource.closestHitShaderModule != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(device, sceneResource.closestHitShaderModule, nullptr);
		sceneResource.closestHitShaderModule = VK_NULL_HANDLE;
	}

	if (sceneResource.raytracePipelineLayout != VK_NULL_HANDLE)
	{
		vkDestroyPipelineLayout(device, sceneResource.raytracePipelineLayout, nullptr);
		sceneResource.raytracePipelineLayout = VK_NULL_HANDLE;
	}

	sceneResource.raytraceDescriptorSet = VK_NULL_HANDLE;

	if (sceneResource.raytraceDescriptorPool != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorPool(device, sceneResource.raytraceDescriptorPool, nullptr);
		sceneResource.raytraceDescriptorPool = VK_NULL_HANDLE;
	}

	if (sceneResource.raytraceDescriptorSetLayout != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorSetLayout(device, sceneResource.raytraceDescriptorSetLayout, nullptr);
		sceneResource.raytraceDescriptorSetLayout = VK_NULL_HANDLE;
	}
}

void ResourceManager::terminate(WorldResource& worldResource, VkDevice device)
{
	VulkanResource::destroyTextureResource(device, worldResource.diffuse);
	VulkanResource::destroyTextureResource(device, worldResource.specular);
	VulkanResource::destroyTextureResource(device, worldResource.lut);
}

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

BufferViewResource* ResourceManager::getBufferViewResource(uint64_t bufferViewHandle)
{
	auto result = bufferViewResources.find(bufferViewHandle);
	if (result == bufferViewResources.end())
	{
		bufferViewResources[bufferViewHandle] = BufferViewResource();
		return &bufferViewResources[bufferViewHandle];
	}
	return &result->second;
}

TextureResource* ResourceManager::getTextureResource(uint64_t textureHandle)
{
	auto result = textureResources.find(textureHandle);
	if (result == textureResources.end())
	{
		textureResources[textureHandle] = TextureResource();
		return &textureResources[textureHandle];
	}
	return &result->second;
}

MaterialResource* ResourceManager::getMaterialResource(uint64_t materialHandle)
{
	auto result = materialResources.find(materialHandle);
	if (result == materialResources.end())
	{
		materialResources[materialHandle] = MaterialResource();
		return &materialResources[materialHandle];
	}
	return &result->second;
}

PrimitiveResource* ResourceManager::getPrimitiveResource(uint64_t primitiveHandle)
{
	auto result = primitiveResources.find(primitiveHandle);
	if (result == primitiveResources.end())
	{
		primitiveResources[primitiveHandle] = PrimitiveResource();
		return &primitiveResources[primitiveHandle];
	}
	return &result->second;
}

GroupResource* ResourceManager::getGroupResource(uint64_t groupHandle)
{
	auto result = groupResources.find(groupHandle);
	if (result == groupResources.end())
	{
		groupResources[groupHandle] = GroupResource();
		return &groupResources[groupHandle];
	}
	return &result->second;
}

InstanceResource* ResourceManager::getInstanceResource(uint64_t instanceHandle)
{
	auto result = instanceResources.find(instanceHandle);
	if (result == instanceResources.end())
	{
		instanceResources[instanceHandle] = InstanceResource();
		return &instanceResources[instanceHandle];
	}
	return &result->second;
}

SceneResource* ResourceManager::getSceneResource(uint64_t sceneHandle)
{
	auto result = sceneResources.find(sceneHandle);
	if (result == sceneResources.end())
	{
		sceneResources[sceneHandle] = SceneResource();
		return &sceneResources[sceneHandle];
	}
	return &result->second;
}

WorldResource* ResourceManager::getWorldResource(uint64_t worldHandle)
{
	auto result = worldResources.find(worldHandle);
	if (result == worldResources.end())
	{
		worldResources[worldHandle] = WorldResource();
		return &worldResources[worldHandle];
	}
	return &result->second;
}

uint64_t ResourceManager::createBufferViewResource(uint64_t externalHandle)
{
	auto result = bufferViewResources.find(externalHandle);
	if (result == bufferViewResources.end())
	{
		bufferViewResources[externalHandle] = BufferViewResource();
	}
	return externalHandle;
}

uint64_t ResourceManager::createTextureResource(uint64_t externalHandle)
{
	auto result = textureResources.find(externalHandle);
	if (result == textureResources.end())
	{
		textureResources[externalHandle] = TextureResource();
	}
	return externalHandle;
}

uint64_t ResourceManager::createMaterialResource(uint64_t externalHandle)
{
	auto result = materialResources.find(externalHandle);
	if (result == materialResources.end())
	{
		materialResources[externalHandle] = MaterialResource();
	}
	return externalHandle;
}

uint64_t ResourceManager::createPrimitiveResource(uint64_t externalHandle)
{
	auto result = primitiveResources.find(externalHandle);
	if (result == primitiveResources.end())
	{
		primitiveResources[externalHandle] = PrimitiveResource();
	}
	return externalHandle;
}

uint64_t ResourceManager::createGroupResource(uint64_t externalHandle)
{
	auto result = groupResources.find(externalHandle);
	if (result == groupResources.end())
	{
		groupResources[externalHandle] = GroupResource();
	}
	return externalHandle;
}

uint64_t ResourceManager::createInstanceResource(uint64_t externalHandle)
{
	auto result = instanceResources.find(externalHandle);
	if (result == instanceResources.end())
	{
		instanceResources[externalHandle] = InstanceResource();
	}
	return externalHandle;
}

uint64_t ResourceManager::createSceneResource(uint64_t externalHandle)
{
	auto result = sceneResources.find(externalHandle);
	if (result == sceneResources.end())
	{
		sceneResources[externalHandle] = SceneResource();
	}
	return externalHandle;
}

uint64_t ResourceManager::createWorldResource(uint64_t externalHandle)
{
	auto result = worldResources.find(externalHandle);
	if (result == worldResources.end())
	{
		worldResources[externalHandle] = WorldResource();
	}
	return externalHandle;
}

bool ResourceManager::deleteBufferViewResource(uint64_t bufferViewHandle, VkDevice device)
{
	BufferViewResource* bufferViewResource = getBufferViewResource(bufferViewHandle);

	if (!bufferViewResource)
	{
		return false;
	}
	terminate(*bufferViewResource, device);
	bufferViewResources.erase(bufferViewHandle);

	return true;
}

bool ResourceManager::deleteTextureResource(uint64_t textureHandle, VkDevice device)
{
	TextureResource* textureResource = getTextureResource(textureHandle);

	if (!textureResource)
	{
		return false;
	}
	terminate(*textureResource, device);
	textureResources.erase(textureHandle);

	return true;
}

bool ResourceManager::deleteMaterialResource(uint64_t materialHandle, VkDevice device)
{
	MaterialResource* materialResource = getMaterialResource(materialHandle);

	if (!materialResource)
	{
		return false;
	}
	terminate(*materialResource, device);
	materialResources.erase(materialHandle);

	return true;
}

bool ResourceManager::deletePrimitiveResource(uint64_t primitiveHandle, VkDevice device)
{
	PrimitiveResource* primitiveResource = getPrimitiveResource(primitiveHandle);

	if (!primitiveResource)
	{
		return false;
	}
	terminate(*primitiveResource, device);
	primitiveResources.erase(primitiveHandle);

	return true;
}

bool ResourceManager::deleteGroupResource(uint64_t groupHandle, VkDevice device)
{
	GroupResource* groupResource = getGroupResource(groupHandle);

	if (!groupResource)
	{
		return false;
	}
	terminate(*groupResource, device);
	groupResources.erase(groupHandle);

	return true;
}

bool ResourceManager::deleteInstanceResource(uint64_t instanceHandle, VkDevice device)
{
	InstanceResource* instanceResource = getInstanceResource(instanceHandle);

	if (!instanceResource)
	{
		return false;
	}
	terminate(*instanceResource, device);
	instanceResources.erase(instanceHandle);

	return true;
}

bool ResourceManager::deleteSceneResource(uint64_t sceneHandle, VkDevice device)
{
	SceneResource* sceneResource = getSceneResource(sceneHandle);

	if (!sceneResource)
	{
		return false;
	}
	terminate(*sceneResource, device);
	sceneResources.erase(sceneHandle);

	return true;
}

bool ResourceManager::deleteWorldResource(uint64_t worldHandle, VkDevice device)
{
	WorldResource* worldResource = getWorldResource(worldHandle);

	if (!worldResource)
	{
		return false;
	}
	terminate(*worldResource, device);
	worldResources.erase(worldHandle);

	return true;
}

void ResourceManager::terminate(VkDevice device)
{
	for (auto it : worldResources)
	{
		terminate(it.second, device);
	}
	worldResources.clear();

	for (auto it : sceneResources)
	{
		terminate(it.second, device);
	}
	sceneResources.clear();

	for (auto it : instanceResources)
	{
		terminate(it.second, device);
	}
	instanceResources.clear();

	for (auto it : groupResources)
	{
		terminate(it.second, device);
	}
	groupResources.clear();

	for (auto it : primitiveResources)
	{
		terminate(it.second, device);
	}
	primitiveResources.clear();

	for (auto it : materialResources)
	{
		terminate(it.second, device);
	}
	materialResources.clear();

	for (auto it : textureResources)
	{
		terminate(it.second, device);
	}
	textureResources.clear();

	for (auto it : bufferViewResources)
	{
		terminate(it.second, device);
	}
	bufferViewResources.clear();
}
