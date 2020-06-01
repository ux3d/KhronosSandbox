#include "ResourceManager.h"

#include "../shader/Shader.h"

void ResourceManager::terminate(SharedDataResource& sharedDataResource, VkDevice device)
{
	VulkanResource::destroyVertexBufferResource(device, sharedDataResource.vertexBufferResource);
}

void ResourceManager::terminate(TextureDataResource& textureResource, VkDevice device)
{
	VulkanResource::destroyTextureResource(device, textureResource.textureResource);
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

void ResourceManager::terminate(GeometryResource& geometryResource, VkDevice device)
{
}

void ResourceManager::terminate(GeometryModelResource& geometryModelResource, VkDevice device)
{
	if (geometryModelResource.graphicsPipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(device, geometryModelResource.graphicsPipeline, nullptr);
		geometryModelResource.graphicsPipeline = VK_NULL_HANDLE;
	}

	if (geometryModelResource.pipelineLayout != VK_NULL_HANDLE)
	{
		vkDestroyPipelineLayout(device, geometryModelResource.pipelineLayout, nullptr);
		geometryModelResource.pipelineLayout = VK_NULL_HANDLE;
	}

	if (geometryModelResource.vertexShaderModule != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(device, geometryModelResource.vertexShaderModule, nullptr);
		geometryModelResource.vertexShaderModule = VK_NULL_HANDLE;
	}

	if (geometryModelResource.fragmentShaderModule != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(device, geometryModelResource.fragmentShaderModule, nullptr);
		geometryModelResource.fragmentShaderModule = VK_NULL_HANDLE;
	}

	//

	VulkanResource::destroyStorageBufferResource(device, geometryModelResource.targetPosition);
	VulkanResource::destroyStorageBufferResource(device, geometryModelResource.targetNormal);
	VulkanResource::destroyStorageBufferResource(device, geometryModelResource.targetTangent);

	//

	VulkanRaytraceResource::destroyBottomLevelResource(device, geometryModelResource.bottomLevelResource);
}

void ResourceManager::terminate(GroupResource& groupResource, VkDevice device)
{
}

void ResourceManager::terminate(InstanceResource& instanceResource, VkDevice device)
{
}

void ResourceManager::terminate(LightResource& lightResource, VkDevice device)
{
	VulkanResource::destroyTextureResource(device, lightResource.diffuse);
	VulkanResource::destroyTextureResource(device, lightResource.specular);
	VulkanResource::destroyTextureResource(device, lightResource.lut);
}

void ResourceManager::terminate(CameraResource& cameraResource, VkDevice device)
{
}

void ResourceManager::terminate(WorldResource& worldResource, VkDevice device)
{
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

TextureDataResource* ResourceManager::getTextureResource(uint64_t textureHandle)
{
	auto result = textureResources.find(textureHandle);
	if (result == textureResources.end())
	{
		textureResources[textureHandle] = TextureDataResource();
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

GeometryResource* ResourceManager::getGeometryResource(uint64_t geometryHandle)
{
	auto result = geometryResources.find(geometryHandle);
	if (result == geometryResources.end())
	{
		geometryResources[geometryHandle] = GeometryResource();
		return &geometryResources[geometryHandle];
	}
	return &result->second;
}

GeometryModelResource* ResourceManager::getGeometryModelResource(uint64_t geometryModelHandle)
{
	auto result = geometryModelResources.find(geometryModelHandle);
	if (result == geometryModelResources.end())
	{
		geometryModelResources[geometryModelHandle] = GeometryModelResource();
		return &geometryModelResources[geometryModelHandle];
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

LightResource* ResourceManager::getLightResource(uint64_t lightHandle)
{
	auto result = lightResources.find(lightHandle);
	if (result == lightResources.end())
	{
		lightResources[lightHandle] = LightResource();
		return &lightResources[lightHandle];
	}
	return &result->second;
}

CameraResource* ResourceManager::getCameraResource(uint64_t cameraHandle)
{
	auto result = cameraResources.find(cameraHandle);
	if (result == cameraResources.end())
	{
		cameraResources[cameraHandle] = CameraResource();
		return &cameraResources[cameraHandle];
	}
	return &result->second;
}

WorldResource* ResourceManager::getWorldResource()
{
	return &worldResource;
}

bool ResourceManager::sharedDataSetData(uint64_t sharedDataHandle, VkDeviceSize size, const void* data)
{
	SharedDataResource* sharedDataResource = getSharedDataResource(sharedDataHandle);

	if (sharedDataResource->finalized)
	{
		return false;
	}

	sharedDataResource->vertexBufferResourceCreateInfo.bufferResourceCreateInfo.size = size;
	sharedDataResource->vertexBufferResourceCreateInfo.data = data;

	return true;
}

bool ResourceManager::sharedDataSetUsage(uint64_t sharedDataHandle, VkBufferUsageFlags usage)
{
	SharedDataResource* sharedDataResource = getSharedDataResource(sharedDataHandle);

	if (sharedDataResource->finalized)
	{
		return false;
	}

	sharedDataResource->vertexBufferResourceCreateInfo.bufferResourceCreateInfo.usage = usage;

	return true;
}

bool ResourceManager::textureResourceSetCreateInformation(uint64_t textureHandle, const TextureResourceCreateInfo& textureResourceCreateInfo)
{
	TextureDataResource* textureDataResource = getTextureResource(textureHandle);

	if (textureDataResource->finalized)
	{
		return false;
	}

	textureDataResource->textureResourceCreateInfo = textureResourceCreateInfo;

	return true;
}

bool ResourceManager::materialResourceSetMaterialParameters(uint64_t materialHandle, const MaterialUniformBuffer& materialUniformBuffer, VkPhysicalDevice physicalDevice, VkDevice device)
{
	MaterialResource* materialResource = getMaterialResource(materialHandle);

	if (materialResource->finalized)
	{
		return false;
	}

	materialResource->materialUniformBuffer = materialUniformBuffer;

	//

	UniformBufferResourceCreateInfo uniformBufferResourceCreateInfo = {};

	uniformBufferResourceCreateInfo.bufferResourceCreateInfo.size = sizeof(MaterialUniformBuffer);
	uniformBufferResourceCreateInfo.bufferResourceCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

	if (!VulkanResource::createUniformBufferResource(physicalDevice, device, materialResource->uniformBufferResource, uniformBufferResourceCreateInfo))
	{
		return false;
	}

	if (!VulkanResource::copyHostToDevice(device, materialResource->uniformBufferResource.bufferResource, &materialResource->materialUniformBuffer, sizeof(materialResource->materialUniformBuffer)))
	{
		return false;
	}

	//

	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
	descriptorSetLayoutBinding = {};
	descriptorSetLayoutBinding.binding = materialResource->binding;
	descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorSetLayoutBinding.descriptorCount = 1;
	descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	materialResource->descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

	VkDescriptorBufferInfo descriptorBufferInfo = {};
	descriptorBufferInfo.buffer = materialResource->uniformBufferResource.bufferResource.buffer;
	descriptorBufferInfo.offset = 0;
	descriptorBufferInfo.range = sizeof(MaterialUniformBuffer);
	materialResource->descriptorBufferInfos.push_back(descriptorBufferInfo);

	materialResource->macros["UNIFORMBUFFER_BINDING"] = std::to_string(materialResource->binding);

	materialResource->binding++;

	return true;
}

bool ResourceManager::materialResourceSetTextureResource(uint64_t materialHandle, uint64_t textureHandle, uint32_t texCoord, const std::string& prefix)
{
	MaterialResource* materialResource = getMaterialResource(materialHandle);

	if (materialResource->finalized)
	{
		return false;
	}

	TextureDataResource* textureResource = getTextureResource(textureHandle);

	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
	descriptorSetLayoutBinding.binding = materialResource->binding;
	descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorSetLayoutBinding.descriptorCount = 1;
	descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	materialResource->descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

	VkDescriptorImageInfo descriptorImageInfo = {};
	descriptorImageInfo.sampler = textureResource->textureResource.samplerResource.sampler;
	descriptorImageInfo.imageView = textureResource->textureResource.imageViewResource.imageView;
	descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	materialResource->descriptorImageInfos.push_back(descriptorImageInfo);

	//

	materialResource->macros[prefix + "_TEXTURE"] = "";
	materialResource->macros[prefix + "_BINDING"] = std::to_string(materialResource->binding);
	materialResource->macros[prefix + "_TEXCOORD"] = HelperShader::getTexCoord(texCoord);

	//

	materialResource->binding++;

	return true;
}

bool ResourceManager::geometryResourceSetAttributesCount(uint64_t geometryHandle, uint32_t attributesCount)
{
	GeometryResource* geometryResource = getGeometryResource(geometryHandle);

	if (geometryResource->finalized)
	{
		return false;
	}

	geometryResource->vertexBuffers.resize(attributesCount);
	geometryResource->vertexBuffersOffsets.resize(attributesCount);

	geometryResource->vertexInputBindingDescriptions.resize(attributesCount);
	geometryResource->vertexInputAttributeDescriptions.resize(attributesCount);

	return true;
}

bool ResourceManager::geometryResourceSetPrimitiveResource(uint64_t geometryHandle, uint32_t typeCount, const std::string& prefix, std::map<std::string, std::string>& macros, VkFormat format, uint32_t stride, VkBuffer buffer, VkDeviceSize offset)
{
	GeometryResource* geometryResource = getGeometryResource(geometryHandle);

	if (geometryResource->finalized)
	{
		return false;
	}

	if (prefix == "POSITION")
	{
		if (typeCount == 3)
		{
			macros[prefix + "_VEC3"] = "";
		}
		else
		{
			return false;
		}

		geometryResource->positionAttributeIndex = geometryResource->attributeIndex;
	}
	else if (prefix == "NORMAL")
	{
		if (typeCount == 3)
		{
			macros[prefix + "_VEC3"] = "";
		}
		else
		{
			return false;
		}

		geometryResource->normalAttributeIndex = geometryResource->attributeIndex;
	}
	else if (prefix == "TANGENT")
	{
		if (typeCount == 4)
		{
			macros[prefix + "_VEC4"] = "";
		}
		else
		{
			return false;
		}

		geometryResource->tangentAttributeIndex = geometryResource->attributeIndex;
	}
	else if (prefix == "TEXCOORD_0")
	{
		if (typeCount == 2)
		{
			macros[prefix + "_VEC2"] = "";
		}
		else
		{
			return false;
		}

		geometryResource->texCoord0AttributeIndex = geometryResource->attributeIndex;
	}
	else if (prefix == "TEXCOORD_1")
	{
		if (typeCount == 2)
		{
			macros[prefix + "_VEC2"] = "";
		}
		else
		{
			return false;
		}

		geometryResource->texCoord1AttributeIndex = geometryResource->attributeIndex;
	}
	else if (prefix == "COLOR_0")
	{
		if (typeCount == 3)
		{
			macros[prefix + "_VEC3"] = "";
		}
		else if (typeCount == 4)
		{
			macros[prefix + "_VEC4"] = "";
		}
		else
		{
			return false;
		}

		geometryResource->color0AttributeIndex = geometryResource->attributeIndex;
	}
	else if (prefix == "JOINTS_0")
	{
		if (typeCount == 4)
		{
			macros[prefix + "_VEC4"] = "";
		}
		else
		{
			return false;
		}

		geometryResource->joints0AttributeIndex = geometryResource->attributeIndex;
	}
	else if (prefix == "JOINTS_1")
	{
		if (typeCount == 4)
		{
			macros[prefix + "_VEC4"] = "";
		}
		else
		{
			return false;
		}

		geometryResource->joints1AttributeIndex = geometryResource->attributeIndex;
	}
	else if (prefix == "WEIGHTS_0")
	{
		if (typeCount == 4)
		{
			macros[prefix + "_VEC4"] = "";
		}
		else
		{
			return false;
		}

		geometryResource->weights0AttributeIndex = geometryResource->attributeIndex;
	}
	else if (prefix == "WEIGHTS_1")
	{
		if (typeCount == 4)
		{
			macros[prefix + "_VEC4"] = "";
		}
		else
		{
			return false;
		}

		geometryResource->weights1AttributeIndex = geometryResource->attributeIndex;
	}
	else
	{
		return false;
	}

	macros[prefix + "_LOC"] = std::to_string(geometryResource->attributeIndex);

	geometryResource->vertexInputBindingDescriptions[geometryResource->attributeIndex].binding = geometryResource->attributeIndex;
	geometryResource->vertexInputBindingDescriptions[geometryResource->attributeIndex].stride = stride;
	geometryResource->vertexInputBindingDescriptions[geometryResource->attributeIndex].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	geometryResource->vertexInputAttributeDescriptions[geometryResource->attributeIndex].binding = geometryResource->attributeIndex;
	geometryResource->vertexInputAttributeDescriptions[geometryResource->attributeIndex].location = geometryResource->attributeIndex;
	geometryResource->vertexInputAttributeDescriptions[geometryResource->attributeIndex].format = format;
	geometryResource->vertexInputAttributeDescriptions[geometryResource->attributeIndex].offset = 0;

	//

	geometryResource->vertexBuffers[geometryResource->attributeIndex] = buffer;
	geometryResource->vertexBuffersOffsets[geometryResource->attributeIndex] = offset;

	//

	geometryResource->attributeIndex++;

	return true;
}

bool ResourceManager::geometryModelResourceSetGeometryResource(uint64_t geometryModelHandle, uint64_t geometryHandle)
{
	GeometryModelResource* geometryModelResource = getGeometryModelResource(geometryModelHandle);

	if (geometryModelResource->finalized)
	{
		return false;
	}

	geometryModelResource->geometryHandle = geometryHandle;

	return true;
}

bool ResourceManager::geometryModelResourceSetMaterialResource(uint64_t geometryModelHandle, uint64_t materialHandle)
{
	GeometryModelResource* geometryModelResource = getGeometryModelResource(geometryModelHandle);

	if (geometryModelResource->finalized)
	{
		return false;
	}

	geometryModelResource->materialHandle = materialHandle;

	return true;
}

bool ResourceManager::groupResourceAddGeometryModelResource(uint64_t groupHandle, uint64_t geometryModelHandle)
{
	GroupResource* groupResource = getGroupResource(groupHandle);

	if (groupResource->finalized)
	{
		return false;
	}

	groupResource->geometryModelHandles.push_back(geometryModelHandle);

	return true;
}

bool ResourceManager::instanceResourceSetWorldMatrix(uint64_t instanceHandle, const glm::mat4& worldMatrix)
{
	InstanceResource* instanceResource = getInstanceResource(instanceHandle);

	if (instanceResource->finalized)
	{
		return false;
	}

	instanceResource->worldMatrix = worldMatrix;

	return true;
}

bool ResourceManager::instanceResourceSetGroupResource(uint64_t instanceHandle, uint64_t groupHandle)
{
	InstanceResource* instanceResource = getInstanceResource(instanceHandle);

	if (instanceResource->finalized)
	{
		return false;
	}

	instanceResource->groupHandle = groupHandle;

	return true;
}

bool ResourceManager::lightResourceSetEnvironmentLight(uint64_t lightHandle, const std::string& environment)
{
	LightResource* lightResource = getLightResource(lightHandle);

	if (lightResource->finalized)
	{
		return false;
	}

	lightResource->environment = environment;

	return true;
}

bool ResourceManager::worldResourceAddInstanceResource(uint64_t instanceHandle)
{
	WorldResource* worldResource = getWorldResource();

	if (worldResource->finalized)
	{
		return false;
	}

	worldResource->instanceHandles.push_back(instanceHandle);

	return true;
}

bool ResourceManager::worldResourceSetLightResource(uint64_t lightHandle)
{
	WorldResource* worldResource = getWorldResource();

	if (worldResource->finalized)
	{
		return false;
	}

	worldResource->lightHandle = lightHandle;

	return true;
}

bool ResourceManager::worldResourceSetCameraResource(uint64_t cameraHandle)
{
	WorldResource* worldResource = getWorldResource();

	if (worldResource->finalized)
	{
		return false;
	}

	worldResource->cameraHandle = cameraHandle;

	return true;
}

bool ResourceManager::sharedDataResourceFinalize(uint64_t sharedDataHandle, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool)
{
	SharedDataResource* sharedDataResource = getSharedDataResource(sharedDataHandle);

	if (sharedDataResource->finalized)
	{
		return false;
	}

	sharedDataResource->vertexBufferResourceCreateInfo.bufferResourceCreateInfo.memoryProperty = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	if (!VulkanResource::createVertexBufferResource(physicalDevice, device, queue, commandPool, sharedDataResource->vertexBufferResource, sharedDataResource->vertexBufferResourceCreateInfo))
	{
		return false;
	}

	sharedDataResource->finalized = true;

	return true;
}

bool ResourceManager::textureResourceFinalize(uint64_t textureHandle, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool)
{
	TextureDataResource* textureDataResource = getTextureResource(textureHandle);

	if (textureDataResource->finalized)
	{
		return false;
	}

	if (!VulkanResource::createTextureResource(physicalDevice, device, queue, commandPool, textureDataResource->textureResource, textureDataResource->textureResourceCreateInfo))
	{
		return false;
	}

	textureDataResource->finalized = true;

	return true;
}

bool ResourceManager::materialResourceFinalize(uint64_t materialHandle, VkDevice device)
{
	MaterialResource* materialResource = getMaterialResource(materialHandle);

	if (materialResource->finalized)
	{
		return false;
	}

	//

	WorldResource* worldResource = getWorldResource();

	if (worldResource->lightHandle > 0)
	{
		LightResource* lightResource = getLightResource(worldResource->lightHandle);

		VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
		descriptorSetLayoutBinding.binding = materialResource->binding;
		descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorSetLayoutBinding.descriptorCount = 1;
		descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		materialResource->descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

		VkDescriptorImageInfo descriptorImageInfo = {};
		descriptorImageInfo.sampler = lightResource->diffuse.samplerResource.sampler;
		descriptorImageInfo.imageView = lightResource->diffuse.imageViewResource.imageView;
		descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		materialResource->descriptorImageInfos.push_back(descriptorImageInfo);

		materialResource->macros["DIFFUSE_BINDING"] = std::to_string(materialResource->binding);

		materialResource->binding++;

		//

		descriptorSetLayoutBinding = {};
		descriptorSetLayoutBinding.binding = materialResource->binding;
		descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorSetLayoutBinding.descriptorCount = 1;
		descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		materialResource->descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

		descriptorImageInfo = {};
		descriptorImageInfo.sampler = lightResource->specular.samplerResource.sampler;
		descriptorImageInfo.imageView = lightResource->specular.imageViewResource.imageView;
		descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		materialResource->descriptorImageInfos.push_back(descriptorImageInfo);

		materialResource->macros["SPECULAR_BINDING"] = std::to_string(materialResource->binding);

		materialResource->binding++;

		//

		descriptorSetLayoutBinding = {};
		descriptorSetLayoutBinding.binding = materialResource->binding;
		descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorSetLayoutBinding.descriptorCount = 1;
		descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		materialResource->descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

		descriptorImageInfo = {};
		descriptorImageInfo.sampler = lightResource->lut.samplerResource.sampler;
		descriptorImageInfo.imageView = lightResource->lut.imageViewResource.imageView;
		descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		materialResource->descriptorImageInfos.push_back(descriptorImageInfo);

		materialResource->macros["LUT_BINDING"] = std::to_string(materialResource->binding);

		materialResource->binding++;
	}
	//

	VkResult result = VK_SUCCESS;

	//

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(materialResource->descriptorSetLayoutBindings.size());
	descriptorSetLayoutCreateInfo.pBindings = materialResource->descriptorSetLayoutBindings.data();

	result = vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &materialResource->descriptorSetLayout);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	//

	std::vector<VkDescriptorPoolSize> descriptorPoolSizes;
	descriptorPoolSizes.resize(materialResource->descriptorSetLayoutBindings.size(), {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1});

	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(materialResource->descriptorSetLayoutBindings.size());
	descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();
	descriptorPoolCreateInfo.maxSets = 1;

	result = vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &materialResource->descriptorPool);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.descriptorPool = materialResource->descriptorPool;
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	descriptorSetAllocateInfo.pSetLayouts = &materialResource->descriptorSetLayout;

	result = vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &materialResource->descriptorSet);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	uint32_t descriptorImageInfosSize = static_cast<uint32_t>(materialResource->descriptorImageInfos.size());
	uint32_t descriptorBufferInfosSize = 1;

	std::vector<VkWriteDescriptorSet> writeDescriptorSets(descriptorImageInfosSize + descriptorBufferInfosSize);

	uint32_t imageIndex = 0;
	uint32_t bufferIndex = 0;
	for (uint32_t k = 0; k < descriptorImageInfosSize + descriptorBufferInfosSize; k++)
	{
		if (materialResource->descriptorSetLayoutBindings[k].descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
		{
			writeDescriptorSets[k].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSets[k].dstSet = materialResource->descriptorSet;
			writeDescriptorSets[k].dstBinding = k;
			writeDescriptorSets[k].dstArrayElement = 0;
			writeDescriptorSets[k].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			writeDescriptorSets[k].descriptorCount = 1;
			writeDescriptorSets[k].pImageInfo = &materialResource->descriptorImageInfos[imageIndex];

			imageIndex++;
		}
		else if (materialResource->descriptorSetLayoutBindings[k].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
		{
			writeDescriptorSets[k].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSets[k].dstSet = materialResource->descriptorSet;
			writeDescriptorSets[k].dstBinding = k;
			writeDescriptorSets[k].dstArrayElement = 0;
			writeDescriptorSets[k].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			writeDescriptorSets[k].descriptorCount = 1;
			writeDescriptorSets[k].pBufferInfo = &materialResource->descriptorBufferInfos[bufferIndex];

			bufferIndex++;
		}
		else
		{
			return false;
		}
	}

	vkUpdateDescriptorSets(device, descriptorImageInfosSize + descriptorBufferInfosSize, writeDescriptorSets.data(), 0, nullptr);

	materialResource->finalized = true;

	return true;
}

bool ResourceManager::geometryResourceFinalize(uint64_t geometryHandle)
{
	GeometryResource* geometryResource = getGeometryResource(geometryHandle);

	if (geometryResource->finalized)
	{
		return false;
	}

	geometryResource->finalized = true;

	return true;
}

bool ResourceManager::geometryModelResourceFinalize(uint64_t geometryModelHandle)
{
	GeometryModelResource* geometryModelResource = getGeometryModelResource(geometryModelHandle);

	if (geometryModelResource->finalized)
	{
		return false;
	}

	geometryModelResource->finalized = true;

	return true;
}

bool ResourceManager::groupResourceFinalize(uint64_t groupHandle)
{
	GroupResource* groupResource = getGroupResource(groupHandle);

	if (groupResource->finalized)
	{
		return false;
	}

	groupResource->finalized = true;

	return true;
}

bool ResourceManager::instanceResourceFinalize(uint64_t instanceHandle)
{
	InstanceResource* instanceResource = getInstanceResource(instanceHandle);

	if (instanceResource->finalized)
	{
		return false;
	}

	instanceResource->finalized = true;

	return true;
}

bool ResourceManager::lightResourceFinalize(uint64_t lightHandle, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool)
{
	LightResource* lightResource = getLightResource(lightHandle);

	if (lightResource->finalized)
	{
		return false;
	}

	// Diffuse

	std::string diffuseFilename = lightResource->environment + "/" + "diffuse.ktx2";
	TextureResourceCreateInfo diffuseMap = {};
	diffuseMap.samplerResourceCreateInfo.minFilter = VK_FILTER_LINEAR;
	diffuseMap.samplerResourceCreateInfo.magFilter = VK_FILTER_LINEAR;
	diffuseMap.samplerResourceCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;

	if(!ImageDataIO::open(diffuseMap.imageDataResources, diffuseFilename))
	{
		return false;
	}

	if (!VulkanResource::createTextureResource(physicalDevice, device, queue, commandPool, lightResource->diffuse, diffuseMap))
	{
		return false;
	}

	// Specular

	std::string specularFilename = lightResource->environment + "/" + "specular.ktx2";
	TextureResourceCreateInfo specularMap = {};
	specularMap.samplerResourceCreateInfo.minFilter = VK_FILTER_LINEAR;
	specularMap.samplerResourceCreateInfo.magFilter = VK_FILTER_LINEAR;
	specularMap.samplerResourceCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

	if(!ImageDataIO::open(specularMap.imageDataResources, specularFilename))
	{
		return false;
	}

	if (!VulkanResource::createTextureResource(physicalDevice, device, queue, commandPool, lightResource->specular, specularMap))
	{
		return false;
	}

	// LUT

	std::string lutFilename = "../Resources/brdf/lut_ggx.png";
	TextureResourceCreateInfo lutMap = {};
	lutMap.samplerResourceCreateInfo.minFilter = VK_FILTER_LINEAR;
	lutMap.samplerResourceCreateInfo.magFilter = VK_FILTER_LINEAR;
	lutMap.samplerResourceCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
	lutMap.samplerResourceCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	lutMap.samplerResourceCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

	if(!ImageDataIO::open(lutMap.imageDataResources, lutFilename))
	{
		return false;
	}

	if (!VulkanResource::createTextureResource(physicalDevice, device, queue, commandPool, lightResource->lut, lutMap))
	{
		return false;
	}


	lightResource->finalized = true;

	return true;
}

bool ResourceManager::cameraResourceFinalize(uint64_t cameraHandle)
{
	CameraResource* cameraResource = getCameraResource(cameraHandle);

	if (cameraResource->finalized)
	{
		return false;
	}

	cameraResource->finalized = true;

	return true;
}

bool ResourceManager::worldResourceFinalize(VkDevice device)
{
	WorldResource* worldResource = getWorldResource();

	if (worldResource->finalized)
	{
		return false;
	}

	//

	for (uint64_t instanceHandle : worldResource->instanceHandles)
	{
		InstanceResource* instanceResource = getInstanceResource(instanceHandle);

		if (!instanceResource->finalized)
		{
			return false;
		}

		//

		GroupResource* groupResource = getGroupResource(instanceResource->groupHandle);

		if (!groupResource->finalized)
		{
			return false;
		}

		for (uint64_t geometryModelHandle : groupResource->geometryModelHandles)
		{
			GeometryModelResource* geometryModelResource = getGeometryModelResource(geometryModelHandle);

			if (!geometryModelResource->finalized)
			{
				return false;
			}

			//

			GeometryResource* geometryResource = getGeometryResource(geometryModelResource->geometryHandle);

			if (!geometryResource->finalized)
			{
				return false;
			}

			MaterialResource* materialResource = getMaterialResource(geometryModelResource->materialHandle);

			if (!materialResource->finalized)
			{
				return false;
			}
		}
	}

	//

	worldResource->finalized = true;

	return true;
}

bool ResourceManager::instanceResourceUpdateWorldMatrix(uint64_t instanceHandle, const glm::mat4& worldMatrix)
{
	InstanceResource* instanceResource = getInstanceResource(instanceHandle);

	if (!instanceResource->finalized)
	{
		return false;
	}

	instanceResource->worldMatrix = worldMatrix;

	return true;
}

bool ResourceManager::sharedDataResourceDelete(uint64_t sharedDataHandle, VkDevice device)
{
	SharedDataResource* sharedDataResource = getSharedDataResource(sharedDataHandle);

	terminate(*sharedDataResource, device);
	sharedDataResources.erase(sharedDataHandle);

	return true;
}

bool ResourceManager::textureResourceDelete(uint64_t textureHandle, VkDevice device)
{
	TextureDataResource* textureResource = getTextureResource(textureHandle);

	terminate(*textureResource, device);
	textureResources.erase(textureHandle);

	return true;
}

bool ResourceManager::materialResourceDelete(uint64_t materialHandle, VkDevice device)
{
	MaterialResource* materialResource = getMaterialResource(materialHandle);

	terminate(*materialResource, device);
	materialResources.erase(materialHandle);

	return true;
}

bool ResourceManager::geometryResourceDelete(uint64_t geometryHandle, VkDevice device)
{
	GeometryResource* geometryResource = getGeometryResource(geometryHandle);

	terminate(*geometryResource, device);
	geometryResources.erase(geometryHandle);

	return true;
}

bool ResourceManager::geometryModelResourceDelete(uint64_t geometryModelHandle, VkDevice device)
{
	GeometryModelResource* geometryModelResource = getGeometryModelResource(geometryModelHandle);

	terminate(*geometryModelResource, device);
	geometryModelResources.erase(geometryModelHandle);

	return true;
}

bool ResourceManager::groupResourceDelete(uint64_t groupHandle, VkDevice device)
{
	GroupResource* groupResource = getGroupResource(groupHandle);

	terminate(*groupResource, device);
	groupResources.erase(groupHandle);

	return true;
}

bool ResourceManager::instanceResourceDelete(uint64_t instanceHandle, VkDevice device)
{
	InstanceResource* instanceResource = getInstanceResource(instanceHandle);

	terminate(*instanceResource, device);
	instanceResources.erase(instanceHandle);

	return true;
}

bool ResourceManager::lightResourceDelete(uint64_t lightHandle, VkDevice device)
{
	LightResource* lightResource = getLightResource(lightHandle);

	terminate(*lightResource, device);
	lightResources.erase(lightHandle);

	return true;
}

bool ResourceManager::cameraResourceDelete(uint64_t cameraHandle, VkDevice device)
{
	CameraResource* cameraResource = getCameraResource(cameraHandle);

	terminate(*cameraResource, device);
	cameraResources.erase(cameraHandle);

	return true;
}

bool ResourceManager::worldResourceDelete(VkDevice device)
{
	WorldResource* worldResource = getWorldResource();

	terminate(*worldResource, device);

	return true;
}

void ResourceManager::terminate(VkDevice device)
{
	terminate(worldResource, device);

	for (auto it : cameraResources)
	{
		terminate(it.second, device);
	}
	cameraResources.clear();

	for (auto it : lightResources)
	{
		terminate(it.second, device);
	}
	lightResources.clear();

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

	for (auto it : geometryModelResources)
	{
		terminate(it.second, device);
	}
	geometryModelResources.clear();

	for (auto it : geometryResources)
	{
		terminate(it.second, device);
	}
	geometryResources.clear();

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
