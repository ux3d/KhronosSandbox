#include "RenderManager.h"

#include "../shader/Shader.h"

void RenderManager::terminate(SharedDataResource& sharedDataResource, VkDevice device)
{
	VulkanResource::destroyVertexBufferResource(device, sharedDataResource.vertexBufferResource);
	VulkanResource::destroyStorageBufferResource(device, sharedDataResource.storageBufferResource);
}

void RenderManager::terminate(TextureDataResource& textureResource, VkDevice device)
{
	VulkanResource::destroyTextureResource(device, textureResource.textureResource);
}

void RenderManager::terminate(MaterialResource& materialResource, VkDevice device)
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

void RenderManager::terminate(GeometryResource& geometryResource, VkDevice device)
{
}

void RenderManager::terminate(GeometryModelResource& geometryModelResource, VkDevice device)
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

	// Shared data is not destroyed here.

	//

	VulkanRaytraceResource::destroyBottomLevelResource(device, geometryModelResource.bottomLevelResource);
}

void RenderManager::terminate(GroupResource& groupResource, VkDevice device)
{
}

void RenderManager::terminate(InstanceResource& instanceResource, VkDevice device)
{
}

void RenderManager::terminate(LightResource& lightResource, VkDevice device)
{
	VulkanResource::destroyTextureResource(device, lightResource.diffuse);
	VulkanResource::destroyTextureResource(device, lightResource.specular);
	VulkanResource::destroyTextureResource(device, lightResource.lut);
}

void RenderManager::terminate(CameraResource& cameraResource, VkDevice device)
{
}

void RenderManager::terminate(WorldResource& worldResource, VkDevice device)
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

RenderManager::RenderManager()
{
}

RenderManager::~RenderManager()
{
}

VkBuffer RenderManager::getBuffer(uint64_t sharedDataHandle)
{
	return getSharedData(sharedDataHandle)->vertexBufferResource.bufferResource.buffer;
}

SharedDataResource* RenderManager::getSharedData(uint64_t sharedDataHandle)
{
	auto result = sharedDataResources.find(sharedDataHandle);
	if (result == sharedDataResources.end())
	{
		sharedDataResources[sharedDataHandle] = SharedDataResource();
		return &sharedDataResources[sharedDataHandle];
	}
	return &result->second;
}

TextureDataResource* RenderManager::getTexture(uint64_t textureHandle)
{
	auto result = textureResources.find(textureHandle);
	if (result == textureResources.end())
	{
		textureResources[textureHandle] = TextureDataResource();
		return &textureResources[textureHandle];
	}
	return &result->second;
}

MaterialResource* RenderManager::getMaterial(uint64_t materialHandle)
{
	auto result = materialResources.find(materialHandle);
	if (result == materialResources.end())
	{
		materialResources[materialHandle] = MaterialResource();
		return &materialResources[materialHandle];
	}
	return &result->second;
}

GeometryResource* RenderManager::getGeometry(uint64_t geometryHandle)
{
	auto result = geometryResources.find(geometryHandle);
	if (result == geometryResources.end())
	{
		geometryResources[geometryHandle] = GeometryResource();
		return &geometryResources[geometryHandle];
	}
	return &result->second;
}

GeometryModelResource* RenderManager::getGeometryModel(uint64_t geometryModelHandle)
{
	auto result = geometryModelResources.find(geometryModelHandle);
	if (result == geometryModelResources.end())
	{
		geometryModelResources[geometryModelHandle] = GeometryModelResource();
		return &geometryModelResources[geometryModelHandle];
	}
	return &result->second;
}

GroupResource* RenderManager::getGroup(uint64_t groupHandle)
{
	auto result = groupResources.find(groupHandle);
	if (result == groupResources.end())
	{
		groupResources[groupHandle] = GroupResource();
		return &groupResources[groupHandle];
	}
	return &result->second;
}

InstanceResource* RenderManager::getInstance(uint64_t instanceHandle)
{
	auto result = instanceResources.find(instanceHandle);
	if (result == instanceResources.end())
	{
		instanceResources[instanceHandle] = InstanceResource();
		return &instanceResources[instanceHandle];
	}
	return &result->second;
}

LightResource* RenderManager::getLight(uint64_t lightHandle)
{
	auto result = lightResources.find(lightHandle);
	if (result == lightResources.end())
	{
		lightResources[lightHandle] = LightResource();
		return &lightResources[lightHandle];
	}
	return &result->second;
}

CameraResource* RenderManager::getCamera(uint64_t cameraHandle)
{
	auto result = cameraResources.find(cameraHandle);
	if (result == cameraResources.end())
	{
		cameraResources[cameraHandle] = CameraResource();
		return &cameraResources[cameraHandle];
	}
	return &result->second;
}

WorldResource* RenderManager::getWorld()
{
	return &worldResource;
}

bool RenderManager::renderSetupVulkan(VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool)
{
	if (this->physicalDevice != VK_NULL_HANDLE ||
		this->device != VK_NULL_HANDLE ||
		this->queue != VK_NULL_HANDLE ||
		this->commandPool != VK_NULL_HANDLE)
	{
		return false;
	}

	this->physicalDevice = physicalDevice;
	this->device = device;
	this->queue = queue;
	this->commandPool = commandPool;

	return true;
}

bool RenderManager::renderUseRaytrace(bool useRaytrace)
{
	this->useRaytrace = useRaytrace;

	return true;
}

bool RenderManager::renderUpdateRaytraceSettings(uint32_t maxDepth, uint32_t specularSamples, uint32_t diffuseSamples)
{
	WorldResource* worldResource = getWorld();

	if (!worldResource->finalized)
	{
		return false;
	}

	worldResource->raytrace.maxDepth = maxDepth;
	worldResource->raytrace.specularSamples = specularSamples;
	worldResource->raytrace.diffuseSamples = diffuseSamples;

	return true;
}

bool RenderManager::sharedDataSetData(uint64_t sharedDataHandle, VkDeviceSize size, const void* data, VkBufferUsageFlags usage)
{
	SharedDataResource* sharedDataResource = getSharedData(sharedDataHandle);

	if (sharedDataResource->finalized)
	{
		return false;
	}

	sharedDataResource->vertexBufferResourceCreateInfo.bufferResourceCreateInfo.size = size;
	sharedDataResource->vertexBufferResourceCreateInfo.data = data;

	sharedDataResource->storageBufferResourceCreateInfo.bufferResourceCreateInfo.size = size;
	sharedDataResource->storageBufferResourceCreateInfo.data = data;

	if (useRaytrace)
	{
		if ((usage & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT) == VK_BUFFER_USAGE_STORAGE_BUFFER_BIT)
		{
			usage |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
		}
		else
		{
			usage |= (VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
		}
	}

	if (((usage & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) == VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) || ((usage & VK_BUFFER_USAGE_INDEX_BUFFER_BIT) == VK_BUFFER_USAGE_INDEX_BUFFER_BIT))
	{
		sharedDataResource->vertexBufferResourceCreateInfo.bufferResourceCreateInfo.usage = usage;
	}
	else
	{
		sharedDataResource->storageBufferResourceCreateInfo.bufferResourceCreateInfo.usage = usage;
	}

	return true;
}

bool RenderManager::textureSetParameters(uint64_t textureHandle, const TextureResourceCreateInfo& textureResourceCreateInfo)
{
	TextureDataResource* textureDataResource = getTexture(textureHandle);

	if (textureDataResource->finalized)
	{
		return false;
	}

	textureDataResource->textureResourceCreateInfo = textureResourceCreateInfo;

	return true;
}

bool RenderManager::materialSetFactorParameters(uint64_t materialHandle, int32_t materialIndex, const MaterialUniformBuffer& materialUniformBuffer)
{
	MaterialResource* materialResource = getMaterial(materialHandle);

	if (materialResource->finalized)
	{
		return false;
	}

	materialResource->materialUniformBuffer = materialUniformBuffer;
	materialResource->materialIndex = materialIndex;

	return true;
}

bool RenderManager::materialSetTexture(uint64_t materialHandle, uint64_t textureHandle, uint32_t texCoord, const std::string& prefix, uint32_t textureIndex)
{
	MaterialResource* materialResource = getMaterial(materialHandle);

	if (materialResource->finalized)
	{
		return false;
	}

	TextureDataResource* textureResource = getTexture(textureHandle);

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

	if (prefix == "BASECOLOR")
	{
		materialResource->baseColorTexture = textureIndex;
	}
	else if (prefix == "METALLICROUGHNESS")
	{
		materialResource->metallicRoughnessTexture = textureIndex;
	}
	else if (prefix == "EMISSIVE")
	{
		materialResource->emissiveTexture = textureIndex;
	}
	else if (prefix == "OCCLUSION")
	{
		materialResource->occlusionTexture = textureIndex;
	}
	else if (prefix == "NORMAL")
	{
		materialResource->normalTexture = textureIndex;
	}

	//

	materialResource->binding++;

	return true;
}

bool RenderManager::geometrySetAttribute(uint64_t geometryHandle, uint32_t count, uint32_t typeCount, const std::string& prefix, VkFormat format, uint32_t stride, uint64_t sharedDataHandle, VkDeviceSize offset, VkDeviceSize range)
{
	GeometryResource* geometryResource = getGeometry(geometryHandle);

	if (geometryResource->finalized)
	{
		return false;
	}

	if (prefix == "POSITION")
	{
		if (typeCount == 3)
		{
			geometryResource->macros[prefix + "_VEC3"] = "";
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
			geometryResource->macros[prefix + "_VEC3"] = "";
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
			geometryResource->macros[prefix + "_VEC4"] = "";
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
			geometryResource->macros[prefix + "_VEC2"] = "";
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
			geometryResource->macros[prefix + "_VEC2"] = "";
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
			geometryResource->macros[prefix + "_VEC3"] = "";
		}
		else if (typeCount == 4)
		{
			geometryResource->macros[prefix + "_VEC4"] = "";
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
			geometryResource->macros[prefix + "_VEC4"] = "";
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
			geometryResource->macros[prefix + "_VEC4"] = "";
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
			geometryResource->macros[prefix + "_VEC4"] = "";
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
			geometryResource->macros[prefix + "_VEC4"] = "";
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

	geometryResource->macros[prefix + "_LOC"] = std::to_string(geometryResource->attributeIndex);

	geometryResource->vertexInputBindingDescriptions.resize(geometryResource->attributeIndex + 1);
	geometryResource->vertexInputBindingDescriptions[geometryResource->attributeIndex].binding = geometryResource->attributeIndex;
	geometryResource->vertexInputBindingDescriptions[geometryResource->attributeIndex].stride = stride;
	geometryResource->vertexInputBindingDescriptions[geometryResource->attributeIndex].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	geometryResource->vertexInputAttributeDescriptions.resize(geometryResource->attributeIndex + 1);
	geometryResource->vertexInputAttributeDescriptions[geometryResource->attributeIndex].binding = geometryResource->attributeIndex;
	geometryResource->vertexInputAttributeDescriptions[geometryResource->attributeIndex].location = geometryResource->attributeIndex;
	geometryResource->vertexInputAttributeDescriptions[geometryResource->attributeIndex].format = format;
	geometryResource->vertexInputAttributeDescriptions[geometryResource->attributeIndex].offset = 0;

	//

	geometryResource->vertexBuffers.resize(geometryResource->attributeIndex + 1);
	geometryResource->vertexBuffers[geometryResource->attributeIndex] = getBuffer(sharedDataHandle);

	geometryResource->vertexBuffersOffsets.resize(geometryResource->attributeIndex + 1);
	geometryResource->vertexBuffersOffsets[geometryResource->attributeIndex] = offset;

	geometryResource->vertexBuffersRanges.resize(geometryResource->attributeIndex + 1);
	geometryResource->vertexBuffersRanges[geometryResource->attributeIndex] = range;

	geometryResource->count = count;

	//

	geometryResource->attributeIndex++;

	return true;
}

bool RenderManager::geometryModelSetGeometry(uint64_t geometryModelHandle, uint64_t geometryHandle)
{
	GeometryModelResource* geometryModelResource = getGeometryModel(geometryModelHandle);

	if (geometryModelResource->finalized)
	{
		return false;
	}

	GeometryResource* geometryResource = getGeometry(geometryHandle);

	if (!geometryResource->finalized)
	{
		return false;
	}

	geometryModelResource->geometryHandle = geometryHandle;

	geometryModelResource->macros.insert(geometryResource->macros.begin(), geometryResource->macros.end());

	return true;
}

bool RenderManager::geometryModelSetMaterial(uint64_t geometryModelHandle, uint64_t materialHandle)
{
	GeometryModelResource* geometryModelResource = getGeometryModel(geometryModelHandle);

	if (geometryModelResource->finalized)
	{
		return false;
	}

	MaterialResource* materialResource = getMaterial(materialHandle);

	if (!materialResource->finalized)
	{
		return false;
	}

	geometryModelResource->materialHandle = materialHandle;

	geometryModelResource->macros.insert(materialResource->macros.begin(), materialResource->macros.end());

	return true;
}

bool RenderManager::geometryModelSetVertexCount(uint64_t geometryModelHandle, uint32_t verticesCount)
{
	GeometryModelResource* geometryModelResource = getGeometryModel(geometryModelHandle);

	if (geometryModelResource->finalized)
	{
		return false;
	}

	geometryModelResource->verticesCount = verticesCount;

	return true;
}

bool RenderManager::geometryModelResourceSetIndices(uint64_t geometryModelHandle, uint32_t indicesCount, VkIndexType indexType, uint64_t sharedDataHandle, uint32_t indexOffset, uint32_t indexRange, uint32_t componentTypeSize)
{
	GeometryModelResource* geometryModelResource = getGeometryModel(geometryModelHandle);

	if (geometryModelResource->finalized)
	{
		return false;
	}

	geometryModelResource->indicesCount = indicesCount;
	geometryModelResource->indexType = indexType;
	geometryModelResource->indexBuffer = getBuffer(sharedDataHandle);
	geometryModelResource->indexOffset = indexOffset;
	geometryModelResource->indexRange = indexRange;
	geometryModelResource->componentTypeSize = componentTypeSize;

	return true;
}

bool RenderManager::geometryModelSetTarget(uint64_t geometryModelHandle, const std::string& targetName, uint64_t sharedDataHandle)
{
	GeometryModelResource* geometryModelResource = getGeometryModel(geometryModelHandle);

	if (geometryModelResource->finalized)
	{
		return false;
	}

	SharedDataResource* sharedDataResource = getSharedData(sharedDataHandle);

	if (!sharedDataResource->finalized)
	{
		return false;
	}

	if (targetName == "POSITION")
	{
		geometryModelResource->targetPosition = &sharedDataResource->storageBufferResource;
	}
	else if (targetName == "NORMAL")
	{
		geometryModelResource->targetNormal = &sharedDataResource->storageBufferResource;
	}
	else if (targetName == "TANGENT")
	{
		geometryModelResource->targetTangent = &sharedDataResource->storageBufferResource;
	}
	else
	{
		return false;
	}

	return true;
}

bool RenderManager::groupAddGeometryModel(uint64_t groupHandle, uint64_t geometryModelHandle)
{
	GroupResource* groupResource = getGroup(groupHandle);

	if (groupResource->finalized)
	{
		return false;
	}

	groupResource->geometryModelHandles.push_back(geometryModelHandle);

	return true;
}

bool RenderManager::instanceSetWorldMatrix(uint64_t instanceHandle, const glm::mat4& worldMatrix)
{
	InstanceResource* instanceResource = getInstance(instanceHandle);

	if (instanceResource->finalized)
	{
		return false;
	}

	instanceResource->worldMatrix = worldMatrix;

	return true;
}

bool RenderManager::instanceSetGroup(uint64_t instanceHandle, uint64_t groupHandle)
{
	InstanceResource* instanceResource = getInstance(instanceHandle);

	if (instanceResource->finalized)
	{
		return false;
	}

	instanceResource->groupHandle = groupHandle;

	return true;
}

bool RenderManager::lightSetEnvironment(uint64_t lightHandle, const std::string& environment)
{
	LightResource* lightResource = getLight(lightHandle);

	if (lightResource->finalized)
	{
		return false;
	}

	lightResource->environment = environment;

	return true;
}

bool RenderManager::worldAddInstance(uint64_t instanceHandle)
{
	WorldResource* worldResource = getWorld();

	if (worldResource->finalized)
	{
		return false;
	}

	worldResource->instanceHandles.push_back(instanceHandle);

	return true;
}

bool RenderManager::worldSetLight(uint64_t lightHandle)
{
	WorldResource* worldResource = getWorld();

	if (worldResource->finalized)
	{
		return false;
	}

	worldResource->lightHandle = lightHandle;

	return true;
}

bool RenderManager::worldSetCamera(uint64_t cameraHandle)
{
	WorldResource* worldResource = getWorld();

	if (worldResource->finalized)
	{
		return false;
	}

	worldResource->cameraHandle = cameraHandle;

	return true;
}

bool RenderManager::sharedDataFinalize(uint64_t sharedDataHandle)
{
	SharedDataResource* sharedDataResource = getSharedData(sharedDataHandle);

	if (sharedDataResource->finalized)
	{
		return false;
	}

	VkBufferUsageFlags usage = sharedDataResource->vertexBufferResourceCreateInfo.bufferResourceCreateInfo.usage;

	if (((usage & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) == VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) || ((usage & VK_BUFFER_USAGE_INDEX_BUFFER_BIT) == VK_BUFFER_USAGE_INDEX_BUFFER_BIT))
	{
		sharedDataResource->vertexBufferResourceCreateInfo.bufferResourceCreateInfo.memoryProperty = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		if (!VulkanResource::createVertexBufferResource(physicalDevice, device, queue, commandPool, sharedDataResource->vertexBufferResource, sharedDataResource->vertexBufferResourceCreateInfo))
		{
			return false;
		}
	}
	else
	{
		sharedDataResource->storageBufferResourceCreateInfo.bufferResourceCreateInfo.memoryProperty = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		if (!VulkanResource::createStorageBufferResource(physicalDevice, device, queue, commandPool, sharedDataResource->storageBufferResource, sharedDataResource->storageBufferResourceCreateInfo))
		{
			return false;
		}
	}

	sharedDataResource->finalized = true;

	return true;
}

bool RenderManager::textureFinalize(uint64_t textureHandle)
{
	TextureDataResource* textureDataResource = getTexture(textureHandle);

	if (textureDataResource->finalized)
	{
		return false;
	}

	if (!VulkanResource::createTextureResource(physicalDevice, device, queue, commandPool, textureDataResource->textureResource, textureDataResource->textureResourceCreateInfo))
	{
		return false;
	}

	//

	WorldResource* worldResource = getWorld();

	VkDescriptorImageInfo descriptorImageInfo = {};

	descriptorImageInfo.sampler = textureDataResource->textureResource.samplerResource.sampler;
	descriptorImageInfo.imageView = textureDataResource->textureResource.imageViewResource.imageView;
	descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	worldResource->descriptorImageInfoTextures.push_back(descriptorImageInfo);

	//

	textureDataResource->finalized = true;

	return true;
}

bool RenderManager::materialFinalize(uint64_t materialHandle)
{
	MaterialResource* materialResource = getMaterial(materialHandle);

	if (materialResource->finalized)
	{
		return false;
	}


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

	//

	WorldResource* worldResource = getWorld();

	if (worldResource->lightHandle > 0)
	{
		LightResource* lightResource = getLight(worldResource->lightHandle);

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

	//

	RaytraceMaterialUniformBuffer uniformBufferRaytrace = {};

	uniformBufferRaytrace.materialUniformBuffer = materialResource->materialUniformBuffer;

	uniformBufferRaytrace.baseColorTexture = materialResource->baseColorTexture;
	uniformBufferRaytrace.metallicRoughnessTexture = materialResource->metallicRoughnessTexture;
	uniformBufferRaytrace.normalTexture = materialResource->normalTexture;
	uniformBufferRaytrace.occlusionTexture = materialResource->occlusionTexture;
	uniformBufferRaytrace.emissiveTexture = materialResource->emissiveTexture;

	worldResource->materialBuffers.push_back(uniformBufferRaytrace);

	//

	materialResource->finalized = true;

	return true;
}

bool RenderManager::geometryFinalize(uint64_t geometryHandle)
{
	GeometryResource* geometryResource = getGeometry(geometryHandle);

	if (geometryResource->finalized)
	{
		return false;
	}

	geometryResource->finalized = true;

	return true;
}

bool RenderManager::geometryModelFinalize(uint64_t geometryModelHandle, uint32_t width, uint32_t height, VkRenderPass renderPass, VkCullModeFlags cullMode, VkSampleCountFlagBits samples)
{
	GeometryModelResource* geometryModelResource = getGeometryModel(geometryModelHandle);

	if (geometryModelResource->finalized)
	{
		return false;
	}

	geometryModelResource->finalized = true;

	//
	// Load the shader code.
	//

	std::string vertexShaderSource = "";
	if (!FileIO::open(vertexShaderSource, "../Resources/shaders/gltf.vert"))
	{
		return false;
	}

	std::string fragmentShaderSource = "";
	if (!FileIO::open(fragmentShaderSource, "../Resources/shaders/gltf.frag"))
	{
		return false;
	}

	//

	std::vector<uint32_t> vertexShaderCode;
	if (!Compiler::buildShader(vertexShaderCode, vertexShaderSource, geometryModelResource->macros, shaderc_vertex_shader))
	{
		return false;
	}

	std::vector<uint32_t> fragmentShaderCode;
	if (!Compiler::buildShader(fragmentShaderCode, fragmentShaderSource, geometryModelResource->macros, shaderc_fragment_shader))
	{
		return false;
	}

	if (!VulkanResource::createShaderModule(geometryModelResource->vertexShaderModule, device, vertexShaderCode))
	{
		return false;
	}

	if (!VulkanResource::createShaderModule(geometryModelResource->fragmentShaderModule, device, fragmentShaderCode))
	{
		return false;
	}

	//

	VkResult result = VK_SUCCESS;

	VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo[2] = {};

	pipelineShaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipelineShaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	pipelineShaderStageCreateInfo[0].module = geometryModelResource->vertexShaderModule;
	pipelineShaderStageCreateInfo[0].pName = "main";

	pipelineShaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipelineShaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	pipelineShaderStageCreateInfo[1].module = geometryModelResource->fragmentShaderModule;
	pipelineShaderStageCreateInfo[1].pName = "main";

	//
	//

	GeometryResource* geometryResource = getGeometry(geometryModelResource->geometryHandle);

	if (!geometryResource->finalized)
	{
		return false;
	}

	VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {};
	pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(geometryResource->vertexInputBindingDescriptions.size());
	pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = geometryResource->vertexInputBindingDescriptions.data();
	pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(geometryResource->vertexInputAttributeDescriptions.size());
	pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = geometryResource->vertexInputAttributeDescriptions.data();

	//
	//

	VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = {};
	pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	//

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)width;
	viewport.height = (float)height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = {0, 0};
	scissor.extent = {width, height};

	VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo = {};
	pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	pipelineViewportStateCreateInfo.viewportCount = 1;
	pipelineViewportStateCreateInfo.pViewports = &viewport;
	pipelineViewportStateCreateInfo.scissorCount = 1;
	pipelineViewportStateCreateInfo.pScissors = &scissor;

	//

	VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo = {};
	pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	pipelineRasterizationStateCreateInfo.cullMode = cullMode;
	pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;

	//

	VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo = {};
	pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	pipelineMultisampleStateCreateInfo.rasterizationSamples = samples;
	pipelineMultisampleStateCreateInfo.minSampleShading = 1.0f;

	//

	VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo = {};
	pipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	pipelineDepthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
	pipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
	pipelineDepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;

	//

	VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState = {};
	pipelineColorBlendAttachmentState.blendEnable = VK_TRUE;
	pipelineColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	pipelineColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	pipelineColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
	pipelineColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	pipelineColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	pipelineColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
	pipelineColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo = {};
	pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	pipelineColorBlendStateCreateInfo.attachmentCount = 1;
	pipelineColorBlendStateCreateInfo.pAttachments = &pipelineColorBlendAttachmentState;

	//

	uint32_t setLayoutCount = 0;
	const VkDescriptorSetLayout* pSetLayouts = nullptr;

	if (geometryModelResource->materialHandle > 0)
	{
		MaterialResource* materialResource = getMaterial(geometryModelResource->materialHandle);

		if (!geometryResource->finalized)
		{
			return false;
		}

		if (materialResource->descriptorSetLayout != VK_NULL_HANDLE)
		{
			setLayoutCount = 1;
			pSetLayouts = &materialResource->descriptorSetLayout;
		}
	}

	VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(ModelViewProjectionUniformPushConstant);

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = setLayoutCount;
	pipelineLayoutCreateInfo.pSetLayouts = pSetLayouts;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
	pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

	result = vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &geometryModelResource->pipelineLayout);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	//

	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
	graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	graphicsPipelineCreateInfo.stageCount = 2;
	graphicsPipelineCreateInfo.pStages = pipelineShaderStageCreateInfo;
	graphicsPipelineCreateInfo.pVertexInputState = &pipelineVertexInputStateCreateInfo;
	graphicsPipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
	graphicsPipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
	graphicsPipelineCreateInfo.pRasterizationState = &pipelineRasterizationStateCreateInfo;
	graphicsPipelineCreateInfo.pMultisampleState = &pipelineMultisampleStateCreateInfo;
	graphicsPipelineCreateInfo.pDepthStencilState = &pipelineDepthStencilStateCreateInfo;
	graphicsPipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
	graphicsPipelineCreateInfo.layout = geometryModelResource->pipelineLayout;
	graphicsPipelineCreateInfo.renderPass = renderPass;

	result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &geometryModelResource->graphicsPipeline);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	if (useRaytrace)
	{
		//
		// Bottom level.
		//

		VkBufferDeviceAddressInfo bufferDeviceAddressInfo = {};
		bufferDeviceAddressInfo.sType  = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;

		bufferDeviceAddressInfo.buffer = geometryResource->vertexBuffers[geometryResource->positionAttributeIndex];
		VkDeviceAddress vertexBufferAddress = vkGetBufferDeviceAddress(device, &bufferDeviceAddressInfo);

		uint32_t vertexCount = geometryResource->count;
		uint32_t primitiveCount = vertexCount / 3;

		VkDeviceAddress vertexIndexBufferAddress = 0;
		if (geometryModelResource->indicesCount >= 0)
		{
			bufferDeviceAddressInfo.buffer = geometryModelResource->indexBuffer;

			vertexIndexBufferAddress = vkGetBufferDeviceAddress(device, &bufferDeviceAddressInfo);

			primitiveCount = geometryModelResource->indicesCount / 3;
		}

		//
		// Bottom level acceleration
		//

		VkFormat positionFormat = geometryResource->vertexInputAttributeDescriptions[geometryResource->positionAttributeIndex].format;
		VkDeviceSize vertexStride = geometryResource->vertexInputBindingDescriptions[geometryResource->positionAttributeIndex].stride;

		BottomLevelResourceCreateInfo bottomLevelResourceCreateInfo = {};
		bottomLevelResourceCreateInfo.indexType = geometryModelResource->indexType;
		bottomLevelResourceCreateInfo.vertexIndexBufferAddress = vertexIndexBufferAddress;
		bottomLevelResourceCreateInfo.vertexCount = vertexCount;
		bottomLevelResourceCreateInfo.vertexFormat = positionFormat;
		bottomLevelResourceCreateInfo.vertexBufferAddress = vertexBufferAddress;
		bottomLevelResourceCreateInfo.vertexStride = vertexStride;
		bottomLevelResourceCreateInfo.primitiveCount = primitiveCount;
		bottomLevelResourceCreateInfo.useHostCommand = false;

		if (!VulkanRaytraceResource::createBottomLevelResource(physicalDevice, device, queue, commandPool, geometryModelResource->bottomLevelResource, bottomLevelResourceCreateInfo))
		{
			return false;
		}
	}


	//

	return true;
}

bool RenderManager::groupFinalize(uint64_t groupHandle)
{
	GroupResource* groupResource = getGroup(groupHandle);

	if (groupResource->finalized)
	{
		return false;
	}

	groupResource->finalized = true;

	return true;
}

bool RenderManager::instanceFinalize(uint64_t instanceHandle)
{
	InstanceResource* instanceResource = getInstance(instanceHandle);

	if (instanceResource->finalized)
	{
		return false;
	}

	instanceResource->finalized = true;

	return true;
}

bool RenderManager::lightFinalize(uint64_t lightHandle)
{
	LightResource* lightResource = getLight(lightHandle);

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

bool RenderManager::cameraFinalize(uint64_t cameraHandle)
{
	CameraResource* cameraResource = getCamera(cameraHandle);

	if (cameraResource->finalized)
	{
		return false;
	}

	cameraResource->finalized = true;

	return true;
}

bool RenderManager::worldFinalize(VkImageView imageView)
{
	WorldResource* worldResource = getWorld();

	if (worldResource->finalized)
	{
		return false;
	}

	//

	if (useRaytrace)
	{
		TopLevelResourceCreateInfo topLevelResourceCreateInfo = {};

		uint32_t primitiveInstanceID = 0;
		int32_t normalInstanceID = 0;
		int32_t tangentInstanceID = 0;
		int32_t texCoord0InstanceID = 0;

		std::vector<VkDescriptorBufferInfo> descriptorBufferInfoIndices;
		std::vector<VkDescriptorBufferInfo> descriptorBufferInfoPosition;
		std::vector<VkDescriptorBufferInfo> descriptorBufferInfoNormal;
		std::vector<VkDescriptorBufferInfo> descriptorBufferInfoTangent;
		std::vector<VkDescriptorBufferInfo> descriptorBufferInfoTexCoord0;

		for (auto it : instanceResources)
		{
			InstanceResource* instanceResource = getInstance(it.first);

			if (instanceResource->groupHandle > 0)
			{
				GroupResource* groupResource = getGroup(instanceResource->groupHandle);

				VkAccelerationStructureDeviceAddressInfoKHR accelerationStructureDeviceAddressInfo = {};
				accelerationStructureDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;

				VkTransformMatrixKHR transformMatrix = {
					instanceResource->worldMatrix[0][0], instanceResource->worldMatrix[1][0], instanceResource->worldMatrix[2][0], instanceResource->worldMatrix[3][0],
					instanceResource->worldMatrix[0][1], instanceResource->worldMatrix[1][1], instanceResource->worldMatrix[2][1], instanceResource->worldMatrix[3][1],
					instanceResource->worldMatrix[0][2], instanceResource->worldMatrix[1][2], instanceResource->worldMatrix[2][2], instanceResource->worldMatrix[3][2]
				};

				for (uint64_t geometryModelHandle : groupResource->geometryModelHandles)
				{
					GeometryModelResource* geometryModelResource = getGeometryModel(geometryModelHandle);

					GeometryResource* geometryResource = getGeometry(geometryModelResource->geometryHandle);

					MaterialResource* materialResource = getMaterial(geometryModelResource->materialHandle);

					//

					accelerationStructureDeviceAddressInfo.accelerationStructure = geometryModelResource->bottomLevelResource.levelResource.accelerationStructureResource.accelerationStructure;
					VkDeviceAddress bottomDeviceAddress = vkGetAccelerationStructureDeviceAddressKHR(device, &accelerationStructureDeviceAddressInfo);

					VkAccelerationStructureInstanceKHR accelerationStructureInstance = {};
					accelerationStructureInstance.transform                              = transformMatrix;
					accelerationStructureInstance.instanceCustomIndex                    = primitiveInstanceID;
					accelerationStructureInstance.mask                                   = 0xFF;
					accelerationStructureInstance.instanceShaderBindingTableRecordOffset = 0;
					accelerationStructureInstance.flags                                  = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
					accelerationStructureInstance.accelerationStructureReference         = bottomDeviceAddress;

					worldResource->accelerationStructureInstances.push_back(accelerationStructureInstance);

					RaytracePrimitiveUniformBuffer primitiveInformation = {};
					primitiveInformation.materialIndex = materialResource->materialIndex;
					primitiveInformation.componentTypeSize = geometryModelResource->componentTypeSize;
					primitiveInformation.worldMatrix = instanceResource->worldMatrix;

					if (geometryResource->normalAttributeIndex >= 0)
					{
						primitiveInformation.normalInstanceID = normalInstanceID;

						normalInstanceID++;
					}
					if (geometryResource->tangentAttributeIndex >= 0)
					{
						primitiveInformation.tangentInstanceID = tangentInstanceID;

						tangentInstanceID++;
					}
					if (geometryResource->texCoord0AttributeIndex >= 0)
					{
						primitiveInformation.texCoord0InstanceID = texCoord0InstanceID;

						texCoord0InstanceID++;
					}

					worldResource->instanceResources.push_back(primitiveInformation);

					primitiveInstanceID++;

					//
					// Gather descriptor buffer info.
					//

					if (geometryModelResource->indicesCount > 0)
					{
						VkDescriptorBufferInfo currentDescriptorBufferInfo = {};

						currentDescriptorBufferInfo.buffer = geometryModelResource->indexBuffer;
						currentDescriptorBufferInfo.offset = geometryModelResource->indexOffset;
						currentDescriptorBufferInfo.range = geometryModelResource->indexRange;

						descriptorBufferInfoIndices.push_back(currentDescriptorBufferInfo);
					}

					if (geometryResource->positionAttributeIndex >= 0)
					{
						VkDescriptorBufferInfo currentDescriptorBufferInfo = {};

						currentDescriptorBufferInfo.buffer = geometryResource->vertexBuffers[geometryResource->positionAttributeIndex];
						currentDescriptorBufferInfo.offset = geometryResource->vertexBuffersOffsets[geometryResource->positionAttributeIndex];
						currentDescriptorBufferInfo.range =  geometryResource->vertexBuffersRanges[geometryResource->positionAttributeIndex];

						descriptorBufferInfoPosition.push_back(currentDescriptorBufferInfo);
					}

					if (geometryResource->normalAttributeIndex >= 0)
					{
						VkDescriptorBufferInfo currentDescriptorBufferInfo = {};

						currentDescriptorBufferInfo.buffer = geometryResource->vertexBuffers[geometryResource->normalAttributeIndex];
						currentDescriptorBufferInfo.offset = geometryResource->vertexBuffersOffsets[geometryResource->normalAttributeIndex];
						currentDescriptorBufferInfo.range =  geometryResource->vertexBuffersRanges[geometryResource->normalAttributeIndex];

						descriptorBufferInfoNormal.push_back(currentDescriptorBufferInfo);
					}

					if (geometryResource->tangentAttributeIndex >= 0)
					{
						VkDescriptorBufferInfo currentDescriptorBufferInfo = {};

						currentDescriptorBufferInfo.buffer = geometryResource->vertexBuffers[geometryResource->tangentAttributeIndex];
						currentDescriptorBufferInfo.offset = geometryResource->vertexBuffersOffsets[geometryResource->tangentAttributeIndex];
						currentDescriptorBufferInfo.range =  geometryResource->vertexBuffersRanges[geometryResource->tangentAttributeIndex];

						descriptorBufferInfoTangent.push_back(currentDescriptorBufferInfo);
					}

					if (geometryResource->texCoord0AttributeIndex >= 0)
					{
						VkDescriptorBufferInfo currentDescriptorBufferInfo = {};

						currentDescriptorBufferInfo.buffer = geometryResource->vertexBuffers[geometryResource->texCoord0AttributeIndex];
						currentDescriptorBufferInfo.offset = geometryResource->vertexBuffersOffsets[geometryResource->texCoord0AttributeIndex];
						currentDescriptorBufferInfo.range =  geometryResource->vertexBuffersRanges[geometryResource->texCoord0AttributeIndex];

						descriptorBufferInfoTexCoord0.push_back(currentDescriptorBufferInfo);
					}
				}
			}
		}
		BufferResourceCreateInfo bufferResourceCreateInfo = {};
		bufferResourceCreateInfo.size = sizeof(VkAccelerationStructureInstanceKHR) * worldResource->accelerationStructureInstances.size();
		bufferResourceCreateInfo.usage = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
		bufferResourceCreateInfo.memoryProperty = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

		if (!VulkanResource::createBufferResource(physicalDevice, device, worldResource->accelerationStructureInstanceBuffer, bufferResourceCreateInfo))
		{
			return false;
		}

		if (!VulkanResource::copyHostToDevice(device, worldResource->accelerationStructureInstanceBuffer, worldResource->accelerationStructureInstances.data(), sizeof(VkAccelerationStructureInstanceKHR) * worldResource->accelerationStructureInstances.size()))
		{
			return false;
		}

		VkBufferDeviceAddressInfo bufferDeviceAddressInfo = {};
		bufferDeviceAddressInfo.sType  = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;

		bufferDeviceAddressInfo.buffer = worldResource->accelerationStructureInstanceBuffer.buffer;
		VkDeviceAddress deviceAddress = vkGetBufferDeviceAddressKHR(device, &bufferDeviceAddressInfo);

		//
		// Top level acceleration
		//

		topLevelResourceCreateInfo.deviceAddress = deviceAddress;
		topLevelResourceCreateInfo.primitiveCount = static_cast<uint32_t>(worldResource->accelerationStructureInstances.size());

		topLevelResourceCreateInfo.useHostCommand = false;

		if (!VulkanRaytraceResource::createTopLevelResource(physicalDevice, device, queue, commandPool, worldResource->topLevelResource, topLevelResourceCreateInfo))
		{
			return false;
		}

		//
		// Gather per instance resources.
		//

		StorageBufferResourceCreateInfo storageBufferResourceCreateInfo = {};
		storageBufferResourceCreateInfo.bufferResourceCreateInfo.size = sizeof(RaytracePrimitiveUniformBuffer) * worldResource->instanceResources.size();
		storageBufferResourceCreateInfo.bufferResourceCreateInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		storageBufferResourceCreateInfo.bufferResourceCreateInfo.memoryProperty = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		storageBufferResourceCreateInfo.data = worldResource->instanceResources.data();

		if (!VulkanResource::createStorageBufferResource(physicalDevice, device, queue, commandPool, worldResource->instanceResourcesStorageBufferResource, storageBufferResourceCreateInfo))
		{
			return false;
		}

		//
		// Gather material resources.
		//

		storageBufferResourceCreateInfo = {};
		storageBufferResourceCreateInfo.bufferResourceCreateInfo.size = sizeof(RaytraceMaterialUniformBuffer) * materialResources.size();
		storageBufferResourceCreateInfo.bufferResourceCreateInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		storageBufferResourceCreateInfo.bufferResourceCreateInfo.memoryProperty = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		storageBufferResourceCreateInfo.data = worldResource->materialBuffers.data();

		if (!VulkanResource::createStorageBufferResource(physicalDevice, device, queue, commandPool, worldResource->materialStorageBufferResource, storageBufferResourceCreateInfo))
		{
			return false;
		}

		//
		// Create required descriptor sets
		//

		uint32_t binding = 0;

		const uint32_t accelerationBinding = binding++;
		const uint32_t outputBinding = binding++;

		const uint32_t diffuseBinding = binding++;
		const uint32_t specularBinding = binding++;
		const uint32_t lutBinding = binding++;

		const uint32_t texturesBinding = binding++;
		const uint32_t materialStorageBufferResourcesBinding = binding++;
		const uint32_t primitivesBinding = binding++;

		const uint32_t indicesBinding = binding++;
		const uint32_t positionBinding = binding++;
		const uint32_t normalBinding = binding++;
		const uint32_t tangentBinding = binding++;
		const uint32_t texCoord0Binding = binding++;

		std::vector<VkDescriptorSetLayoutBinding> raytraceDescriptorSetLayoutBindings = {};

		VkDescriptorSetLayoutBinding raytraceDescriptorSetLayoutBinding = {};
		raytraceDescriptorSetLayoutBinding.binding         = accelerationBinding;
		raytraceDescriptorSetLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
		raytraceDescriptorSetLayoutBinding.descriptorCount = 1;
		raytraceDescriptorSetLayoutBinding.stageFlags      = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
		raytraceDescriptorSetLayoutBindings.push_back(raytraceDescriptorSetLayoutBinding);

		raytraceDescriptorSetLayoutBinding = {};
		raytraceDescriptorSetLayoutBinding.binding         = outputBinding;
		raytraceDescriptorSetLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		raytraceDescriptorSetLayoutBinding.descriptorCount = 1;
		raytraceDescriptorSetLayoutBinding.stageFlags      = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
		raytraceDescriptorSetLayoutBindings.push_back(raytraceDescriptorSetLayoutBinding);

		raytraceDescriptorSetLayoutBinding = {};
		raytraceDescriptorSetLayoutBinding.binding         = diffuseBinding;
		raytraceDescriptorSetLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		raytraceDescriptorSetLayoutBinding.descriptorCount = 1;
		raytraceDescriptorSetLayoutBinding.stageFlags      = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR;
		raytraceDescriptorSetLayoutBindings.push_back(raytraceDescriptorSetLayoutBinding);

		raytraceDescriptorSetLayoutBinding = {};
		raytraceDescriptorSetLayoutBinding.binding         = specularBinding;
		raytraceDescriptorSetLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		raytraceDescriptorSetLayoutBinding.descriptorCount = 1;
		raytraceDescriptorSetLayoutBinding.stageFlags      = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR;
		raytraceDescriptorSetLayoutBindings.push_back(raytraceDescriptorSetLayoutBinding);

		raytraceDescriptorSetLayoutBinding = {};
		raytraceDescriptorSetLayoutBinding.binding         = lutBinding;
		raytraceDescriptorSetLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		raytraceDescriptorSetLayoutBinding.descriptorCount = 1;
		raytraceDescriptorSetLayoutBinding.stageFlags      = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR;
		raytraceDescriptorSetLayoutBindings.push_back(raytraceDescriptorSetLayoutBinding);

		if (worldResource->descriptorImageInfoTextures.size() > 0)
		{
			raytraceDescriptorSetLayoutBinding = {};
			raytraceDescriptorSetLayoutBinding.binding         = texturesBinding;
			raytraceDescriptorSetLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			raytraceDescriptorSetLayoutBinding.descriptorCount = static_cast<uint32_t>(worldResource->descriptorImageInfoTextures.size());
			raytraceDescriptorSetLayoutBinding.stageFlags      = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
			raytraceDescriptorSetLayoutBindings.push_back(raytraceDescriptorSetLayoutBinding);
		}

		raytraceDescriptorSetLayoutBinding = {};
		raytraceDescriptorSetLayoutBinding.binding         = materialStorageBufferResourcesBinding;
		raytraceDescriptorSetLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		raytraceDescriptorSetLayoutBinding.descriptorCount = 1;
		raytraceDescriptorSetLayoutBinding.stageFlags      = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
		raytraceDescriptorSetLayoutBindings.push_back(raytraceDescriptorSetLayoutBinding);

		raytraceDescriptorSetLayoutBinding = {};
		raytraceDescriptorSetLayoutBinding.binding         = primitivesBinding;
		raytraceDescriptorSetLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		raytraceDescriptorSetLayoutBinding.descriptorCount = 1;
		raytraceDescriptorSetLayoutBinding.stageFlags      = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
		raytraceDescriptorSetLayoutBindings.push_back(raytraceDescriptorSetLayoutBinding);

		raytraceDescriptorSetLayoutBinding = {};
		raytraceDescriptorSetLayoutBinding.binding         = indicesBinding;
		raytraceDescriptorSetLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		raytraceDescriptorSetLayoutBinding.descriptorCount = static_cast<uint32_t>(descriptorBufferInfoIndices.size());
		raytraceDescriptorSetLayoutBinding.stageFlags      = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
		raytraceDescriptorSetLayoutBindings.push_back(raytraceDescriptorSetLayoutBinding);

		raytraceDescriptorSetLayoutBinding = {};
		raytraceDescriptorSetLayoutBinding.binding         = positionBinding;
		raytraceDescriptorSetLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		raytraceDescriptorSetLayoutBinding.descriptorCount = static_cast<uint32_t>(descriptorBufferInfoPosition.size());
		raytraceDescriptorSetLayoutBinding.stageFlags      = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
		raytraceDescriptorSetLayoutBindings.push_back(raytraceDescriptorSetLayoutBinding);

		if (descriptorBufferInfoNormal.size() > 0)
		{
			raytraceDescriptorSetLayoutBinding = {};
			raytraceDescriptorSetLayoutBinding.binding         = normalBinding;
			raytraceDescriptorSetLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			raytraceDescriptorSetLayoutBinding.descriptorCount = static_cast<uint32_t>(descriptorBufferInfoNormal.size());
			raytraceDescriptorSetLayoutBinding.stageFlags      = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
			raytraceDescriptorSetLayoutBindings.push_back(raytraceDescriptorSetLayoutBinding);
		}

		if (descriptorBufferInfoTangent.size() > 0)
		{
			raytraceDescriptorSetLayoutBinding = {};
			raytraceDescriptorSetLayoutBinding.binding         = tangentBinding;
			raytraceDescriptorSetLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			raytraceDescriptorSetLayoutBinding.descriptorCount = static_cast<uint32_t>(descriptorBufferInfoTangent.size());
			raytraceDescriptorSetLayoutBinding.stageFlags      = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
			raytraceDescriptorSetLayoutBindings.push_back(raytraceDescriptorSetLayoutBinding);
		}

		if (descriptorBufferInfoTexCoord0.size() > 0)
		{
			raytraceDescriptorSetLayoutBinding = {};
			raytraceDescriptorSetLayoutBinding.binding         = texCoord0Binding;
			raytraceDescriptorSetLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			raytraceDescriptorSetLayoutBinding.descriptorCount = static_cast<uint32_t>(descriptorBufferInfoTexCoord0.size());
			raytraceDescriptorSetLayoutBinding.stageFlags      = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
			raytraceDescriptorSetLayoutBindings.push_back(raytraceDescriptorSetLayoutBinding);
		}

		VkDescriptorSetLayoutCreateInfo raytraceDescriptorSetLayoutCreateInfo = {};
		raytraceDescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		raytraceDescriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(raytraceDescriptorSetLayoutBindings.size());
		raytraceDescriptorSetLayoutCreateInfo.pBindings = raytraceDescriptorSetLayoutBindings.data();

		VkResult result = vkCreateDescriptorSetLayout(device, &raytraceDescriptorSetLayoutCreateInfo, nullptr, &worldResource->raytraceDescriptorSetLayout);
		if (result != VK_SUCCESS)
		{
			Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

			return false;
		}

		std::vector<VkDescriptorPoolSize> raytraceDescriptorPoolSizes;

		VkDescriptorPoolSize raytraceDescriptorPoolSize = {};
		raytraceDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
		raytraceDescriptorPoolSize.descriptorCount = 1;
		raytraceDescriptorPoolSizes.push_back(raytraceDescriptorPoolSize);

		raytraceDescriptorPoolSize = {};
		raytraceDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		raytraceDescriptorPoolSize.descriptorCount = 1;
		raytraceDescriptorPoolSizes.push_back(raytraceDescriptorPoolSize);

		raytraceDescriptorPoolSize = {};
		raytraceDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		raytraceDescriptorPoolSize.descriptorCount = 1;
		raytraceDescriptorPoolSizes.push_back(raytraceDescriptorPoolSize);

		raytraceDescriptorPoolSize = {};
		raytraceDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		raytraceDescriptorPoolSize.descriptorCount = 1;
		raytraceDescriptorPoolSizes.push_back(raytraceDescriptorPoolSize);

		raytraceDescriptorPoolSize = {};
		raytraceDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		raytraceDescriptorPoolSize.descriptorCount = 1;
		raytraceDescriptorPoolSizes.push_back(raytraceDescriptorPoolSize);

		if (worldResource->descriptorImageInfoTextures.size() > 0)
		{
			raytraceDescriptorPoolSize = {};
			raytraceDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			raytraceDescriptorPoolSize.descriptorCount = static_cast<uint32_t>(worldResource->descriptorImageInfoTextures.size());
			raytraceDescriptorPoolSizes.push_back(raytraceDescriptorPoolSize);
		}

		raytraceDescriptorPoolSize = {};
		raytraceDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		raytraceDescriptorPoolSize.descriptorCount = 1;
		raytraceDescriptorPoolSizes.push_back(raytraceDescriptorPoolSize);

		raytraceDescriptorPoolSize = {};
		raytraceDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		raytraceDescriptorPoolSize.descriptorCount = 1;
		raytraceDescriptorPoolSizes.push_back(raytraceDescriptorPoolSize);

		raytraceDescriptorPoolSize = {};
		raytraceDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		raytraceDescriptorPoolSize.descriptorCount = static_cast<uint32_t>(descriptorBufferInfoIndices.size());
		raytraceDescriptorPoolSizes.push_back(raytraceDescriptorPoolSize);

		raytraceDescriptorPoolSize = {};
		raytraceDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		raytraceDescriptorPoolSize.descriptorCount = static_cast<uint32_t>(descriptorBufferInfoPosition.size());
		raytraceDescriptorPoolSizes.push_back(raytraceDescriptorPoolSize);

		if (descriptorBufferInfoNormal.size() > 0)
		{
			raytraceDescriptorPoolSize = {};
			raytraceDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			raytraceDescriptorPoolSize.descriptorCount = static_cast<uint32_t>(descriptorBufferInfoNormal.size());
			raytraceDescriptorPoolSizes.push_back(raytraceDescriptorPoolSize);
		}

		if (descriptorBufferInfoTangent.size() > 0)
		{
			raytraceDescriptorPoolSize = {};
			raytraceDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			raytraceDescriptorPoolSize.descriptorCount = static_cast<uint32_t>(descriptorBufferInfoTangent.size());
			raytraceDescriptorPoolSizes.push_back(raytraceDescriptorPoolSize);
		}

		if (descriptorBufferInfoTexCoord0.size() > 0)
		{
			raytraceDescriptorPoolSize = {};
			raytraceDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			raytraceDescriptorPoolSize.descriptorCount = static_cast<uint32_t>(descriptorBufferInfoTexCoord0.size());
			raytraceDescriptorPoolSizes.push_back(raytraceDescriptorPoolSize);
		}

		VkDescriptorPoolCreateInfo raytraceDescriptorPoolCreateInfo = {};
		raytraceDescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		raytraceDescriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(raytraceDescriptorPoolSizes.size());
		raytraceDescriptorPoolCreateInfo.pPoolSizes = raytraceDescriptorPoolSizes.data();
		raytraceDescriptorPoolCreateInfo.maxSets = 1;

		result = vkCreateDescriptorPool(device, &raytraceDescriptorPoolCreateInfo, nullptr, &worldResource->raytraceDescriptorPool);
		if (result != VK_SUCCESS)
		{
			Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

			return false;
		}

		VkDescriptorSetAllocateInfo raytraceDescriptorSetAllocateInfo = {};
		raytraceDescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		raytraceDescriptorSetAllocateInfo.descriptorPool = worldResource->raytraceDescriptorPool;
		raytraceDescriptorSetAllocateInfo.descriptorSetCount = 1;
		raytraceDescriptorSetAllocateInfo.pSetLayouts = &worldResource->raytraceDescriptorSetLayout;

		result = vkAllocateDescriptorSets(device, &raytraceDescriptorSetAllocateInfo, &worldResource->raytraceDescriptorSet);
		if (result != VK_SUCCESS)
		{
			Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

			return false;
		}

		//
		//

		VkWriteDescriptorSetAccelerationStructureKHR writeDescriptorSetAccelerationStructure = {};
		writeDescriptorSetAccelerationStructure.sType                      = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
		writeDescriptorSetAccelerationStructure.accelerationStructureCount = 1;
		writeDescriptorSetAccelerationStructure.pAccelerationStructures    = &worldResource->topLevelResource.levelResource.accelerationStructureResource.accelerationStructure;

		VkDescriptorImageInfo descriptorImageInfo = {};
		descriptorImageInfo.sampler = VK_NULL_HANDLE;
		descriptorImageInfo.imageView = imageView;
		descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

		VkDescriptorBufferInfo descriptorBufferInfo = {};
		descriptorBufferInfo.buffer = worldResource->materialStorageBufferResource.bufferResource.buffer;
		descriptorBufferInfo.offset = 0;
		descriptorBufferInfo.range = sizeof(RaytraceMaterialUniformBuffer) * materialResources.size();

		VkDescriptorBufferInfo descriptorBufferInfo2 = {};
		descriptorBufferInfo2.buffer = worldResource->instanceResourcesStorageBufferResource.bufferResource.buffer;
		descriptorBufferInfo2.offset = 0;
		descriptorBufferInfo2.range = sizeof(RaytracePrimitiveUniformBuffer) * worldResource->instanceResources.size();

		LightResource* lightResource = getLight(worldResource->lightHandle);

		VkDescriptorImageInfo descriptorImageInfoDiffuse = {};
		descriptorImageInfoDiffuse.sampler = lightResource->diffuse.samplerResource.sampler;
		descriptorImageInfoDiffuse.imageView = lightResource->diffuse.imageViewResource.imageView;
		descriptorImageInfoDiffuse.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkDescriptorImageInfo descriptorImageInfoSpecular = {};
		descriptorImageInfoSpecular.sampler = lightResource->specular.samplerResource.sampler;
		descriptorImageInfoSpecular.imageView = lightResource->specular.imageViewResource.imageView;
		descriptorImageInfoSpecular.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkDescriptorImageInfo descriptorImageInfoLut = {};
		descriptorImageInfoLut.sampler = lightResource->lut.samplerResource.sampler;
		descriptorImageInfoLut.imageView = lightResource->lut.imageViewResource.imageView;
		descriptorImageInfoLut.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		std::vector<VkWriteDescriptorSet> writeDescriptorSets;

		VkWriteDescriptorSet writeDescriptorSet = {};
		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet          = worldResource->raytraceDescriptorSet;
		writeDescriptorSet.dstBinding      = accelerationBinding;
		writeDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.pNext           = &writeDescriptorSetAccelerationStructure;	// Chained
		writeDescriptorSets.push_back(writeDescriptorSet);

		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = worldResource->raytraceDescriptorSet;
		writeDescriptorSet.dstBinding = outputBinding;
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.pImageInfo = &descriptorImageInfo;
		writeDescriptorSets.push_back(writeDescriptorSet);

		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = worldResource->raytraceDescriptorSet;
		writeDescriptorSet.dstBinding = diffuseBinding;
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.pImageInfo = &descriptorImageInfoDiffuse;
		writeDescriptorSets.push_back(writeDescriptorSet);

		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = worldResource->raytraceDescriptorSet;
		writeDescriptorSet.dstBinding = specularBinding;
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.pImageInfo = &descriptorImageInfoSpecular;
		writeDescriptorSets.push_back(writeDescriptorSet);

		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = worldResource->raytraceDescriptorSet;
		writeDescriptorSet.dstBinding = lutBinding;
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.pImageInfo = &descriptorImageInfoLut;
		writeDescriptorSets.push_back(writeDescriptorSet);

		if (worldResource->descriptorImageInfoTextures.size() > 0)
		{
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.dstSet = worldResource->raytraceDescriptorSet;
			writeDescriptorSet.dstBinding = texturesBinding;
			writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			writeDescriptorSet.descriptorCount = static_cast<uint32_t>(worldResource->descriptorImageInfoTextures.size());
			writeDescriptorSet.pImageInfo = worldResource->descriptorImageInfoTextures.data();
			writeDescriptorSets.push_back(writeDescriptorSet);
		}

		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = worldResource->raytraceDescriptorSet;
		writeDescriptorSet.dstBinding = materialStorageBufferResourcesBinding;
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;
		writeDescriptorSets.push_back(writeDescriptorSet);

		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = worldResource->raytraceDescriptorSet;
		writeDescriptorSet.dstBinding = primitivesBinding;
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.pBufferInfo = &descriptorBufferInfo2;
		writeDescriptorSets.push_back(writeDescriptorSet);

		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = worldResource->raytraceDescriptorSet;
		writeDescriptorSet.dstBinding = indicesBinding;
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		writeDescriptorSet.descriptorCount = static_cast<uint32_t>(descriptorBufferInfoIndices.size());
		writeDescriptorSet.pBufferInfo = descriptorBufferInfoIndices.data();
		writeDescriptorSets.push_back(writeDescriptorSet);

		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = worldResource->raytraceDescriptorSet;
		writeDescriptorSet.dstBinding = positionBinding;
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		writeDescriptorSet.descriptorCount = static_cast<uint32_t>(descriptorBufferInfoPosition.size());
		writeDescriptorSet.pBufferInfo = descriptorBufferInfoPosition.data();
		writeDescriptorSets.push_back(writeDescriptorSet);

		if (descriptorBufferInfoNormal.size() > 0)
		{
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.dstSet = worldResource->raytraceDescriptorSet;
			writeDescriptorSet.dstBinding = normalBinding;
			writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			writeDescriptorSet.descriptorCount = static_cast<uint32_t>(descriptorBufferInfoNormal.size());
			writeDescriptorSet.pBufferInfo = descriptorBufferInfoNormal.data();
			writeDescriptorSets.push_back(writeDescriptorSet);
		}

		if (descriptorBufferInfoTangent.size() > 0)
		{
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.dstSet = worldResource->raytraceDescriptorSet;
			writeDescriptorSet.dstBinding = tangentBinding;
			writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			writeDescriptorSet.descriptorCount = static_cast<uint32_t>(descriptorBufferInfoTangent.size());
			writeDescriptorSet.pBufferInfo = descriptorBufferInfoTangent.data();
			writeDescriptorSets.push_back(writeDescriptorSet);
		}

		if (descriptorBufferInfoTexCoord0.size() > 0)
		{
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.dstSet = worldResource->raytraceDescriptorSet;
			writeDescriptorSet.dstBinding = texCoord0Binding;
			writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			writeDescriptorSet.descriptorCount = static_cast<uint32_t>(descriptorBufferInfoTexCoord0.size());
			writeDescriptorSet.pBufferInfo = descriptorBufferInfoTexCoord0.data();
			writeDescriptorSets.push_back(writeDescriptorSet);
		}

		vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);

		//
		// Creating ray tracing raytracePipeline.
		//

		VkPushConstantRange pushConstantRange = {};
	    pushConstantRange.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
	    pushConstantRange.offset = 0;
	    pushConstantRange.size = sizeof(RaytraceUniformPushConstant);

		VkPipelineLayoutCreateInfo raytracePipelineLayoutCreateInfo = {};
		raytracePipelineLayoutCreateInfo.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		raytracePipelineLayoutCreateInfo.setLayoutCount = 1;
		raytracePipelineLayoutCreateInfo.pSetLayouts    = &worldResource->raytraceDescriptorSetLayout;
		raytracePipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		raytracePipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

		result = vkCreatePipelineLayout(device, &raytracePipelineLayoutCreateInfo, nullptr, &worldResource->raytracePipelineLayout);
		if (result != VK_SUCCESS)
		{
			Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

			return false;
		}

		//
		// Build the required raytracing shaders.
		//

		std::map<std::string, std::string> macros;

		macros["ACCELERATION_BINDING"] = std::to_string(accelerationBinding);
		macros["OUTPUT_BINDING"] = std::to_string(outputBinding);

		macros["DIFFUSE_BINDING"] = std::to_string(diffuseBinding);
		macros["SPECULAR_BINDING"] = std::to_string(specularBinding);
		macros["LUT_BINDING"] = std::to_string(lutBinding);

		macros["TEXTURES_BINDING"] = std::to_string(texturesBinding);
		macros["MATERIALS_BINDING"] = std::to_string(materialStorageBufferResourcesBinding);
		macros["PRIMITIVES_BINDING"] = std::to_string(primitivesBinding);

		macros["INDICES_BINDING"] = std::to_string(indicesBinding);
		macros["POSITION_BINDING"] = std::to_string(positionBinding);
		macros["NORMAL_BINDING"] = std::to_string(normalBinding);
		macros["TANGENT_BINDING"] = std::to_string(tangentBinding);
		macros["TEXCOORD0_BINDING"] = std::to_string(texCoord0Binding);

		if (worldResource->descriptorImageInfoTextures.size() > 0)
		{
			macros["HAS_TEXTURES"] = "";
		}

		if (descriptorBufferInfoNormal.size() > 0)
		{
			macros["NORMAL_VEC3"] = "";
		}
		if (descriptorBufferInfoTangent.size() > 0)
		{
			macros["TANGENT_VEC4"] = "";
		}
		if (descriptorBufferInfoTexCoord0.size() > 0)
		{
			macros["TEXCOORD0_VEC2"] = "";
		}

		std::string rayGenShaderSource = "";
		if (!FileIO::open(rayGenShaderSource, "../Resources/shaders/gltf.rgen"))
		{
			return false;
		}

		std::vector<uint32_t> rayGenShaderCode;
		if (!Compiler::buildShader(rayGenShaderCode, rayGenShaderSource, macros, shaderc_raygen_shader))
		{
			return false;
		}

		if (!VulkanResource::createShaderModule(worldResource->rayGenShaderModule, device, rayGenShaderCode))
		{
			return false;
		}

		//

		std::string missShaderSource = "";
		if (!FileIO::open(missShaderSource, "../Resources/shaders/gltf.rmiss"))
		{
			return false;
		}

		std::vector<uint32_t> missShaderCode;
		if (!Compiler::buildShader(missShaderCode, missShaderSource, macros, shaderc_miss_shader))
		{
			return false;
		}

		if (!VulkanResource::createShaderModule(worldResource->missShaderModule, device, missShaderCode))
		{
			return false;
		}

		//

		std::string closestHitShaderSource = "";
		if (!FileIO::open(closestHitShaderSource, "../Resources/shaders/gltf.rchit"))
		{
			return false;
		}

		std::vector<uint32_t> closestHitShaderCode;
		if (!Compiler::buildShader(closestHitShaderCode, closestHitShaderSource, macros, shaderc_closesthit_shader))
		{
			return false;
		}

		if (!VulkanResource::createShaderModule(worldResource->closestHitShaderModule, device, closestHitShaderCode))
		{
			return false;
		}

		//
		//

		std::vector<VkPipelineShaderStageCreateInfo> raytracePipelineShaderStageCreateInfos;
		raytracePipelineShaderStageCreateInfos.resize(3);

		raytracePipelineShaderStageCreateInfos[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		raytracePipelineShaderStageCreateInfos[0].stage = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
		raytracePipelineShaderStageCreateInfos[0].module = worldResource->rayGenShaderModule;
		raytracePipelineShaderStageCreateInfos[0].pName = "main";

		raytracePipelineShaderStageCreateInfos[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		raytracePipelineShaderStageCreateInfos[1].stage = VK_SHADER_STAGE_MISS_BIT_KHR;
		raytracePipelineShaderStageCreateInfos[1].module = worldResource->missShaderModule;
		raytracePipelineShaderStageCreateInfos[1].pName = "main";

		raytracePipelineShaderStageCreateInfos[2].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		raytracePipelineShaderStageCreateInfos[2].stage = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
		raytracePipelineShaderStageCreateInfos[2].module = worldResource->closestHitShaderModule;
		raytracePipelineShaderStageCreateInfos[2].pName = "main";

		//

		std::vector<VkRayTracingShaderGroupCreateInfoKHR> rayTracingShaderGroupCreateInfos = {};
		rayTracingShaderGroupCreateInfos.resize(3);

		rayTracingShaderGroupCreateInfos[0] = {};
		rayTracingShaderGroupCreateInfos[0].sType              = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
		rayTracingShaderGroupCreateInfos[0].type 			   = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
		rayTracingShaderGroupCreateInfos[0].generalShader      = 0;
		rayTracingShaderGroupCreateInfos[0].closestHitShader   = VK_SHADER_UNUSED_KHR;
		rayTracingShaderGroupCreateInfos[0].anyHitShader       = VK_SHADER_UNUSED_KHR;
		rayTracingShaderGroupCreateInfos[0].intersectionShader = VK_SHADER_UNUSED_KHR;

		rayTracingShaderGroupCreateInfos[1] = {};
		rayTracingShaderGroupCreateInfos[1].sType              = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
		rayTracingShaderGroupCreateInfos[1].type 			   = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
		rayTracingShaderGroupCreateInfos[1].generalShader      = 1;
		rayTracingShaderGroupCreateInfos[1].closestHitShader   = VK_SHADER_UNUSED_KHR;
		rayTracingShaderGroupCreateInfos[1].anyHitShader       = VK_SHADER_UNUSED_KHR;
		rayTracingShaderGroupCreateInfos[1].intersectionShader = VK_SHADER_UNUSED_KHR;

		rayTracingShaderGroupCreateInfos[2] = {};
		rayTracingShaderGroupCreateInfos[2].sType              = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
		rayTracingShaderGroupCreateInfos[2].type 			   = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
		rayTracingShaderGroupCreateInfos[2].generalShader      = VK_SHADER_UNUSED_KHR;
		rayTracingShaderGroupCreateInfos[2].closestHitShader   = 2;
		rayTracingShaderGroupCreateInfos[2].anyHitShader       = VK_SHADER_UNUSED_KHR;
		rayTracingShaderGroupCreateInfos[2].intersectionShader = VK_SHADER_UNUSED_KHR;

		VkRayTracingPipelineCreateInfoKHR rayTracingPipelineCreateInfo = {};
		rayTracingPipelineCreateInfo.sType             = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
		rayTracingPipelineCreateInfo.stageCount        = static_cast<uint32_t>(raytracePipelineShaderStageCreateInfos.size());
		rayTracingPipelineCreateInfo.pStages           = raytracePipelineShaderStageCreateInfos.data();
		rayTracingPipelineCreateInfo.groupCount        = static_cast<uint32_t>(rayTracingShaderGroupCreateInfos.size());
		rayTracingPipelineCreateInfo.pGroups           = rayTracingShaderGroupCreateInfos.data();
		rayTracingPipelineCreateInfo.maxRecursionDepth = 2;
		rayTracingPipelineCreateInfo.layout            = worldResource->raytracePipelineLayout;
		rayTracingPipelineCreateInfo.libraries.sType   = VK_STRUCTURE_TYPE_PIPELINE_LIBRARY_CREATE_INFO_KHR;

		result = vkCreateRayTracingPipelinesKHR(device, VK_NULL_HANDLE, 1, &rayTracingPipelineCreateInfo, nullptr, &worldResource->raytracePipeline);
		if (result != VK_SUCCESS)
		{
			Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

			return false;
		}

		//
		// Build up shader binding table.
		//

		VkPhysicalDeviceProperties2 physicalDeviceProperties2 = {};
		physicalDeviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		VkPhysicalDeviceRayTracingPropertiesKHR physicalDeviceRayTracingProperties = {};
		physicalDeviceRayTracingProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PROPERTIES_KHR;

		physicalDeviceProperties2.pNext = &physicalDeviceRayTracingProperties;
		vkGetPhysicalDeviceProperties2(physicalDevice, &physicalDeviceProperties2);

		bufferResourceCreateInfo = {};
		bufferResourceCreateInfo.size = physicalDeviceRayTracingProperties.shaderGroupHandleSize * rayTracingShaderGroupCreateInfos.size();
		bufferResourceCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_RAY_TRACING_BIT_KHR;
		bufferResourceCreateInfo.memoryProperty = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

		if (!VulkanResource::createBufferResource(physicalDevice, device, worldResource->shaderBindingBufferResource, bufferResourceCreateInfo))
		{
			return false;
		}
		worldResource->size = physicalDeviceRayTracingProperties.shaderGroupHandleSize;

		std::vector<uint8_t> rayTracingShaderGroupHandles(bufferResourceCreateInfo.size);

		result = vkGetRayTracingShaderGroupHandlesKHR(device, worldResource->raytracePipeline, 0, static_cast<uint32_t>(rayTracingShaderGroupCreateInfos.size()), rayTracingShaderGroupHandles.size(), rayTracingShaderGroupHandles.data());
		if (result != VK_SUCCESS)
		{
			Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

			return false;
		}

		if (!VulkanResource::copyHostToDevice(device, worldResource->shaderBindingBufferResource, rayTracingShaderGroupHandles.data(), rayTracingShaderGroupHandles.size()))
		{
			return false;
		}
	}


	//

	for (uint64_t instanceHandle : worldResource->instanceHandles)
	{
		InstanceResource* instanceResource = getInstance(instanceHandle);

		if (!instanceResource->finalized)
		{
			return false;
		}

		//

		GroupResource* groupResource = getGroup(instanceResource->groupHandle);

		if (!groupResource->finalized)
		{
			return false;
		}

		for (uint64_t geometryModelHandle : groupResource->geometryModelHandles)
		{
			GeometryModelResource* geometryModelResource = getGeometryModel(geometryModelHandle);

			if (!geometryModelResource->finalized)
			{
				return false;
			}

			//

			GeometryResource* geometryResource = getGeometry(geometryModelResource->geometryHandle);

			if (!geometryResource->finalized)
			{
				return false;
			}

			MaterialResource* materialResource = getMaterial(geometryModelResource->materialHandle);

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

bool RenderManager::worldGetCamera(uint64_t& cameraHandle)
{
	WorldResource* worldResource = getWorld();

	if (worldResource->cameraHandle > 0)
	{
		cameraHandle = worldResource->cameraHandle;

		return true;
	}

	return false;
}

bool RenderManager::instanceUpdateWorldMatrix(uint64_t instanceHandle, const glm::mat4& worldMatrix)
{
	InstanceResource* instanceResource = getInstance(instanceHandle);

	if (!instanceResource->finalized)
	{
		return false;
	}

	instanceResource->worldMatrix = worldMatrix;

	return true;
}

bool RenderManager::cameraUpdateProjectionMatrix(uint64_t cameraHandle, const glm::mat4& projectionMatrix)
{
	CameraResource* cameraResource = getCamera(cameraHandle);

	if (!cameraResource->finalized)
	{
		return false;
	}

	cameraResource->projection = projectionMatrix;

	WorldResource* worldResource = getWorld();

	worldResource->viewProjection.projection = projectionMatrix;
	worldResource->raytrace.inverseViewProjection.inverseProjection = glm::inverse(projectionMatrix);

	return true;
}

bool RenderManager::cameraUpdateViewMatrix(uint64_t cameraHandle, const glm::mat4& viewMatrix)
{
	CameraResource* cameraResource = getCamera(cameraHandle);

	if (!cameraResource->finalized)
	{
		return false;
	}

	cameraResource->view = viewMatrix;

	WorldResource* worldResource = getWorld();

	worldResource->viewProjection.view = viewMatrix;
	worldResource->raytrace.inverseViewProjection.inverseView = glm::inverse(viewMatrix);

	return true;
}

bool RenderManager::sharedDataDelete(uint64_t sharedDataHandle)
{
	SharedDataResource* sharedDataResource = getSharedData(sharedDataHandle);

	terminate(*sharedDataResource, device);
	sharedDataResources.erase(sharedDataHandle);

	return true;
}

bool RenderManager::textureDelete(uint64_t textureHandle)
{
	TextureDataResource* textureResource = getTexture(textureHandle);

	terminate(*textureResource, device);
	textureResources.erase(textureHandle);

	return true;
}

bool RenderManager::materialDelete(uint64_t materialHandle)
{
	MaterialResource* materialResource = getMaterial(materialHandle);

	terminate(*materialResource, device);
	materialResources.erase(materialHandle);

	return true;
}

bool RenderManager::geometryDelete(uint64_t geometryHandle)
{
	GeometryResource* geometryResource = getGeometry(geometryHandle);

	terminate(*geometryResource, device);
	geometryResources.erase(geometryHandle);

	return true;
}

bool RenderManager::geometryModelDelete(uint64_t geometryModelHandle)
{
	GeometryModelResource* geometryModelResource = getGeometryModel(geometryModelHandle);

	terminate(*geometryModelResource, device);
	geometryModelResources.erase(geometryModelHandle);

	return true;
}

bool RenderManager::groupDelete(uint64_t groupHandle)
{
	GroupResource* groupResource = getGroup(groupHandle);

	terminate(*groupResource, device);
	groupResources.erase(groupHandle);

	return true;
}

bool RenderManager::instanceDelete(uint64_t instanceHandle)
{
	InstanceResource* instanceResource = getInstance(instanceHandle);

	terminate(*instanceResource, device);
	instanceResources.erase(instanceHandle);

	return true;
}

bool RenderManager::lightDelete(uint64_t lightHandle)
{
	LightResource* lightResource = getLight(lightHandle);

	terminate(*lightResource, device);
	lightResources.erase(lightHandle);

	return true;
}

bool RenderManager::cameraDelete(uint64_t cameraHandle)
{
	CameraResource* cameraResource = getCamera(cameraHandle);

	terminate(*cameraResource, device);
	cameraResources.erase(cameraHandle);

	return true;
}

bool RenderManager::worldDelete()
{
	WorldResource* worldResource = getWorld();

	terminate(*worldResource, device);

	return true;
}

void RenderManager::terminate()
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

	//

	useRaytrace = false;

	physicalDevice = VK_NULL_HANDLE;
	device = VK_NULL_HANDLE;
	queue = VK_NULL_HANDLE;
	commandPool = VK_NULL_HANDLE;
}

void RenderManager::rasterize(VkCommandBuffer commandBuffer, uint32_t frameIndex, DrawMode drawMode)
{
	WorldResource* worldResource = getWorld();

	for (size_t i = 0; i < worldResource->instanceHandles.size(); i++)
	{
		InstanceResource* instanceResource = getInstance(worldResource->instanceHandles[i]);

		GroupResource* groupResource = getGroup(instanceResource->groupHandle);

		for (size_t k = 0; k < groupResource->geometryModelHandles.size(); k++)
		{
			GeometryModelResource* geometryModelResource = getGeometryModel(groupResource->geometryModelHandles[k]);

			GeometryResource* geometryResource = getGeometry(geometryModelResource->geometryHandle);

			MaterialResource* materialResource = getMaterial(geometryModelResource->materialHandle);

			//

			if (materialResource->materialUniformBuffer.alphaMode == 2 && drawMode == OPAQUE)
			{
				return;
			}
			else if (materialResource->materialUniformBuffer.alphaMode != 2 && drawMode == TRANSPARENT)
			{
				return;
			}

			//

			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, geometryModelResource->graphicsPipeline);

			if (materialResource->descriptorSet != VK_NULL_HANDLE)
			{
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, geometryModelResource->pipelineLayout, 0, 1, &materialResource->descriptorSet, 0, nullptr);
			}

			vkCmdPushConstants(commandBuffer, geometryModelResource->pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(worldResource->viewProjection), &worldResource->viewProjection);
			vkCmdPushConstants(commandBuffer, geometryModelResource->pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(worldResource->viewProjection), sizeof(instanceResource->worldMatrix), &instanceResource->worldMatrix);

			if (geometryModelResource->indexBuffer != VK_NULL_HANDLE)
			{
				vkCmdBindIndexBuffer(commandBuffer, geometryModelResource->indexBuffer, geometryModelResource->indexOffset, geometryModelResource->indexType);
			}

			vkCmdBindVertexBuffers(commandBuffer, 0, static_cast<uint32_t>(geometryResource->vertexBuffers.size()), geometryResource->vertexBuffers.data(), geometryResource->vertexBuffersOffsets.data());

			if (geometryModelResource->indexBuffer != VK_NULL_HANDLE)
			{
				vkCmdDrawIndexed(commandBuffer, geometryModelResource->indicesCount, 1, 0, 0, 0);
			}
			else
			{
				vkCmdDraw(commandBuffer, geometryModelResource->verticesCount, 1, 0, 0);
			}
		}
	}
}

void RenderManager::raytrace(VkCommandBuffer commandBuffer, uint32_t frameIndex, uint32_t width, uint32_t height)
{
	WorldResource* worldResource = getWorld();

	vkCmdPushConstants(commandBuffer, worldResource->raytracePipelineLayout, VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, 0, sizeof(worldResource->raytrace), &worldResource->raytrace);

	VkStridedBufferRegionKHR rayGenStridedBufferRegion = {};
	rayGenStridedBufferRegion.buffer = worldResource->shaderBindingBufferResource.buffer;
	rayGenStridedBufferRegion.offset = worldResource->size * 0;
	rayGenStridedBufferRegion.size   = worldResource->size;

	VkStridedBufferRegionKHR missStridedBufferRegion = {};
	missStridedBufferRegion.buffer = worldResource->shaderBindingBufferResource.buffer;
	missStridedBufferRegion.offset = worldResource->size * 1;
	missStridedBufferRegion.size   = worldResource->size;

	VkStridedBufferRegionKHR closestHitStridedBufferRegion = {};
	closestHitStridedBufferRegion.buffer = worldResource->shaderBindingBufferResource.buffer;
	closestHitStridedBufferRegion.offset = worldResource->size * 2;
	closestHitStridedBufferRegion.size   = worldResource->size;

	VkStridedBufferRegionKHR callableStridedBufferRegion = {};

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, worldResource->raytracePipeline);

	if (worldResource->raytraceDescriptorSet != VK_NULL_HANDLE)
	{
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, worldResource->raytracePipelineLayout, 0, 1, &worldResource->raytraceDescriptorSet, 0, 0);
	}

	vkCmdTraceRaysKHR(commandBuffer, &rayGenStridedBufferRegion, &missStridedBufferRegion, &closestHitStridedBufferRegion, &callableStridedBufferRegion, width, height, 1);
}

