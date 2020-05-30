#include "ResourceManager.h"

void ResourceManager::terminate(SharedDataResource& sharedDataResource, VkDevice device)
{
	VulkanResource::destroyVertexBufferResource(device, sharedDataResource.vertexBufferResource);
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

void ResourceManager::terminate(WorldResource& worldResource, VkDevice device)
{
	VulkanResource::destroyTextureResource(device, worldResource.diffuse);
	VulkanResource::destroyTextureResource(device, worldResource.specular);
	VulkanResource::destroyTextureResource(device, worldResource.lut);

	VulkanResource::destroyStorageBufferResource(device, worldResource.instanceResourcesStorageBufferResource);
	VulkanResource::destroyStorageBufferResource(device, worldResource.materialStorageBufferResource);

	VulkanRaytraceResource::destroyTopLevelResource(device, worldResource.topLevelResource);
	VulkanResource::destroyBufferResource(device, worldResource.accelerationStructureInstanceBuffer);

	VulkanResource::destroyBufferResource(device, worldResource.shaderBindingBufferResource);
	worldResource.size = 0;

	worldResource.accelerationStructureInstances.clear();

	if (worldResource.raytracePipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(device, worldResource.raytracePipeline, nullptr);
		worldResource.raytracePipeline = VK_NULL_HANDLE;
	}

	if (worldResource.rayGenShaderModule != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(device, worldResource.rayGenShaderModule, nullptr);
		worldResource.rayGenShaderModule = VK_NULL_HANDLE;
	}

	if (worldResource.missShaderModule != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(device, worldResource.missShaderModule, nullptr);
		worldResource.missShaderModule = VK_NULL_HANDLE;
	}

	if (worldResource.closestHitShaderModule != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(device, worldResource.closestHitShaderModule, nullptr);
		worldResource.closestHitShaderModule = VK_NULL_HANDLE;
	}

	if (worldResource.raytracePipelineLayout != VK_NULL_HANDLE)
	{
		vkDestroyPipelineLayout(device, worldResource.raytracePipelineLayout, nullptr);
		worldResource.raytracePipelineLayout = VK_NULL_HANDLE;
	}

	worldResource.raytraceDescriptorSet = VK_NULL_HANDLE;

	if (worldResource.raytraceDescriptorPool != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorPool(device, worldResource.raytraceDescriptorPool, nullptr);
		worldResource.raytraceDescriptorPool = VK_NULL_HANDLE;
	}

	if (worldResource.raytraceDescriptorSetLayout != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorSetLayout(device, worldResource.raytraceDescriptorSetLayout, nullptr);
		worldResource.raytraceDescriptorSetLayout = VK_NULL_HANDLE;
	}
}

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

VkBuffer ResourceManager::getBuffer(uint64_t sharedDataHandle)
{
	return getSharedDataResource(sharedDataHandle)->vertexBufferResource.bufferResource.buffer;
}

SharedDataResource* ResourceManager::getSharedDataResource(uint64_t sharedDataHandle)
{
	auto result = sharedDataResources.find(sharedDataHandle);
	if (result == sharedDataResources.end())
	{
		sharedDataResources[sharedDataHandle] = SharedDataResource();
		return &sharedDataResources[sharedDataHandle];
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

bool ResourceManager::finalizeSharedDataResource(uint64_t externalHandle, VkDeviceSize size, const void* data, VkBufferUsageFlags usage, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool)
{
	auto it = sharedDataResources.find(externalHandle);
	if (it == sharedDataResources.end())
	{
		sharedDataResources[externalHandle] = SharedDataResource();
	}

	//

	VertexBufferResourceCreateInfo vertexBufferResourceCreateInfo = {};

	vertexBufferResourceCreateInfo.bufferResourceCreateInfo.size = size;
	vertexBufferResourceCreateInfo.bufferResourceCreateInfo.usage = usage;
	vertexBufferResourceCreateInfo.bufferResourceCreateInfo.memoryProperty = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	vertexBufferResourceCreateInfo.data = data;

	if (!VulkanResource::createVertexBufferResource(physicalDevice, device, queue, commandPool, sharedDataResources[externalHandle].vertexBufferResource, vertexBufferResourceCreateInfo))
	{
		return false;
	}

	//

	return true;
}

bool ResourceManager::finalizeTextureResource(uint64_t externalHandle, const TextureResourceCreateInfo& textureResourceCreateInfo, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool)
{
	auto it = textureResources.find(externalHandle);
	if (it == textureResources.end())
	{
		textureResources[externalHandle] = TextureResource();
	}

	//

	if (!VulkanResource::createTextureResource(physicalDevice, device, queue, commandPool, textureResources[externalHandle], textureResourceCreateInfo))
	{
		return false;
	}

	//

	return true;
}

bool ResourceManager::finalizeMaterialResource(uint64_t externalHandle, VkDevice device)
{
	auto it = materialResources.find(externalHandle);
	if (it == materialResources.end())
	{
		materialResources[externalHandle] = MaterialResource();
	}

	//

	VkResult result = VK_SUCCESS;

	//

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(materialResources[externalHandle].descriptorSetLayoutBindings.size());
	descriptorSetLayoutCreateInfo.pBindings = materialResources[externalHandle].descriptorSetLayoutBindings.data();

	result = vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &materialResources[externalHandle].descriptorSetLayout);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	//

	std::vector<VkDescriptorPoolSize> descriptorPoolSizes;
	descriptorPoolSizes.resize(materialResources[externalHandle].descriptorSetLayoutBindings.size(), {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1});

	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(materialResources[externalHandle].descriptorSetLayoutBindings.size());
	descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();
	descriptorPoolCreateInfo.maxSets = 1;

	result = vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &materialResources[externalHandle].descriptorPool);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.descriptorPool = materialResources[externalHandle].descriptorPool;
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	descriptorSetAllocateInfo.pSetLayouts = &materialResources[externalHandle].descriptorSetLayout;

	result = vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &materialResources[externalHandle].descriptorSet);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	uint32_t descriptorImageInfosSize = static_cast<uint32_t>(materialResources[externalHandle].descriptorImageInfos.size());
	uint32_t descriptorBufferInfosSize = 1;

	std::vector<VkWriteDescriptorSet> writeDescriptorSets(descriptorImageInfosSize + descriptorBufferInfosSize);

	for (uint32_t k = 0; k < descriptorImageInfosSize; k++)
	{
		writeDescriptorSets[k].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[k].dstSet = materialResources[externalHandle].descriptorSet;
		writeDescriptorSets[k].dstBinding = k;
		writeDescriptorSets[k].dstArrayElement = 0;
		writeDescriptorSets[k].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSets[k].descriptorCount = 1;
		writeDescriptorSets[k].pImageInfo = &materialResources[externalHandle].descriptorImageInfos[k];
	}

	for (uint32_t k = descriptorImageInfosSize; k < descriptorImageInfosSize + descriptorBufferInfosSize; k++)
	{
		writeDescriptorSets[k].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[k].dstSet = materialResources[externalHandle].descriptorSet;
		writeDescriptorSets[k].dstBinding = k;
		writeDescriptorSets[k].dstArrayElement = 0;
		writeDescriptorSets[k].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSets[k].descriptorCount = 1;
		writeDescriptorSets[k].pBufferInfo = &materialResources[externalHandle].descriptorBufferInfo;

		// No more at this point of time.
		break;
	}

	vkUpdateDescriptorSets(device, descriptorImageInfosSize + descriptorBufferInfosSize, writeDescriptorSets.data(), 0, nullptr);

	return true;
}

bool ResourceManager::finalizePrimitiveResource(uint64_t externalHandle)
{
	auto it = primitiveResources.find(externalHandle);
	if (it == primitiveResources.end())
	{
		primitiveResources[externalHandle] = PrimitiveResource();
	}
	return true;
}

bool ResourceManager::finalizeGroupResource(uint64_t externalHandle)
{
	auto it = groupResources.find(externalHandle);
	if (it == groupResources.end())
	{
		groupResources[externalHandle] = GroupResource();
	}
	return true;
}

bool ResourceManager::finalizeInstanceResource(uint64_t externalHandle)
{
	auto it = instanceResources.find(externalHandle);
	if (it == instanceResources.end())
	{
		instanceResources[externalHandle] = InstanceResource();
	}
	return true;
}

bool ResourceManager::finalizeWorldResource(uint64_t externalHandle)
{
	auto it = worldResources.find(externalHandle);
	if (it == worldResources.end())
	{
		worldResources[externalHandle] = WorldResource();
	}
	return true;
}

bool ResourceManager::deleteSharedDataResource(uint64_t sharedDataHandle, VkDevice device)
{
	SharedDataResource* sharedDataResource = getSharedDataResource(sharedDataHandle);

	if (!sharedDataResource)
	{
		return false;
	}
	terminate(*sharedDataResource, device);
	sharedDataResources.erase(sharedDataHandle);

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

	for (auto it : sharedDataResources)
	{
		terminate(it.second, device);
	}
	sharedDataResources.clear();
}
