#include "RenderManager.h"

#include <cstring>

#include "../shader/Shader.h"

void RenderManager::terminate(SharedDataResource& sharedDataResource, VkDevice device)
{
	VulkanResource::destroyVertexBufferResource(device, sharedDataResource.vertexBufferResource);
	VulkanResource::destroyUniformBufferResource(device, sharedDataResource.uniformBufferResource);
	VulkanResource::destroyStorageBufferResource(device, sharedDataResource.storageBufferResource);
}

void RenderManager::terminate(TextureDataResource& textureResource, VkDevice device)
{
	VulkanResource::destroyTextureResource(device, textureResource.textureResource);
}

void RenderManager::terminate(MaterialResource& materialResource, VkDevice device)
{
	VulkanResource::destroyUniformBufferResource(device, materialResource.uniformBufferResource);
}

void RenderManager::terminate(GeometryResource& geometryResource, VkDevice device)
{
}

void RenderManager::terminate(GeometryModelResource& geometryModelResource, VkDevice device)
{
}

void RenderManager::terminate(GroupResource& groupResource, VkDevice device)
{
}

void RenderManager::terminate(InstanceResource& instanceResource, VkDevice device)
{
	for (size_t geometryModelIndex = 0; geometryModelIndex < instanceResource.instanceContainers.size(); geometryModelIndex++)
	{
		if (instanceResource.instanceContainers[geometryModelIndex].graphicsPipeline != VK_NULL_HANDLE)
		{
			vkDestroyPipeline(device, instanceResource.instanceContainers[geometryModelIndex].graphicsPipeline, nullptr);
			instanceResource.instanceContainers[geometryModelIndex].graphicsPipeline = VK_NULL_HANDLE;
		}

		if (instanceResource.instanceContainers[geometryModelIndex].pipelineLayout != VK_NULL_HANDLE)
		{
			vkDestroyPipelineLayout(device, instanceResource.instanceContainers[geometryModelIndex].pipelineLayout, nullptr);
			instanceResource.instanceContainers[geometryModelIndex].pipelineLayout = VK_NULL_HANDLE;
		}

		if (instanceResource.instanceContainers[geometryModelIndex].vertexShaderModule != VK_NULL_HANDLE)
		{
			vkDestroyShaderModule(device, instanceResource.instanceContainers[geometryModelIndex].vertexShaderModule, nullptr);
			instanceResource.instanceContainers[geometryModelIndex].vertexShaderModule = VK_NULL_HANDLE;
		}

		if (instanceResource.instanceContainers[geometryModelIndex].fragmentShaderModule != VK_NULL_HANDLE)
		{
			vkDestroyShaderModule(device, instanceResource.instanceContainers[geometryModelIndex].fragmentShaderModule, nullptr);
			instanceResource.instanceContainers[geometryModelIndex].fragmentShaderModule = VK_NULL_HANDLE;
		}

		//

		// Descriptor sets do not have to be freed, as managed by pool.
		instanceResource.instanceContainers[geometryModelIndex].descriptorSet = VK_NULL_HANDLE;

		if (instanceResource.instanceContainers[geometryModelIndex].descriptorPool != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorPool(device, instanceResource.instanceContainers[geometryModelIndex].descriptorPool, nullptr);
			instanceResource.instanceContainers[geometryModelIndex].descriptorPool = VK_NULL_HANDLE;
		}

		if (instanceResource.instanceContainers[geometryModelIndex].descriptorSetLayout != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorSetLayout(device, instanceResource.instanceContainers[geometryModelIndex].descriptorSetLayout, nullptr);
			instanceResource.instanceContainers[geometryModelIndex].descriptorSetLayout = VK_NULL_HANDLE;
		}
	}
}

void RenderManager::terminate(LightResource& lightResource, VkDevice device)
{
	VulkanResource::destroyTextureResource(device, lightResource.lambertian);
	VulkanResource::destroyTextureResource(device, lightResource.ggx);
	VulkanResource::destroyTextureResource(device, lightResource.ggxLUT);
}

void RenderManager::terminate(CameraResource& cameraResource, VkDevice device)
{
}

void RenderManager::terminate(WorldResource& worldResource, VkDevice device)
{
}

RenderManager::RenderManager()
{
}

RenderManager::~RenderManager()
{
}

VkBuffer RenderManager::getBuffer(uint64_t sharedDataHandle)
{
	SharedDataResource* sharedDataResource = getSharedData(sharedDataHandle);

	if (sharedDataResource->vertexBufferResource.bufferResource.buffer != VK_NULL_HANDLE)
	{
		return sharedDataResource->vertexBufferResource.bufferResource.buffer;
	}
	else if (sharedDataResource->uniformBufferResource.bufferResource.buffer != VK_NULL_HANDLE)
	{
		return sharedDataResource->uniformBufferResource.bufferResource.buffer;
	}
	else if (sharedDataResource->storageBufferResource.bufferResource.buffer != VK_NULL_HANDLE)
	{
		return sharedDataResource->storageBufferResource.bufferResource.buffer;
	}

	return VK_NULL_HANDLE;
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

	vkGetPhysicalDeviceProperties(physicalDevice, &this->physicalDeviceProperties);

	return true;
}

bool RenderManager::renderSetDimension(uint32_t width, uint32_t height)
{
	this->width = width;
	this->height = height;

	return true;
}

bool RenderManager::renderSetRenderPass(VkRenderPass renderPass)
{
	this->renderPass = renderPass;

	return true;
}

bool RenderManager::renderSetSamples(VkSampleCountFlagBits samples)
{
	this->samples = samples;

	return true;
}

bool RenderManager::renderSetFrames(uint32_t frames)
{
	this->frames = frames;

	return true;
}

bool RenderManager::sharedDataSetData(uint64_t sharedDataHandle, VkDeviceSize size, const void* data, VkBufferUsageFlags usage)
{
	SharedDataResource* sharedDataResource = getSharedData(sharedDataHandle);

	if (!sharedDataResource->created || sharedDataResource->finalized)
	{
		return false;
	}

	sharedDataResource->usage = usage;
	sharedDataResource->size = size;

	sharedDataResource->vertexBufferResourceCreateInfo.bufferResourceCreateInfo.size = size;
	sharedDataResource->vertexBufferResourceCreateInfo.data = data;

	sharedDataResource->uniformBufferResourceCreateInfo.bufferResourceCreateInfo.size = size;
	sharedDataResource->uniformBufferResourceCreateInfo.data = data;

	sharedDataResource->storageBufferResourceCreateInfo.bufferResourceCreateInfo.size = size;
	sharedDataResource->storageBufferResourceCreateInfo.data = data;

	if (((usage & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) == VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) || ((usage & VK_BUFFER_USAGE_INDEX_BUFFER_BIT) == VK_BUFFER_USAGE_INDEX_BUFFER_BIT))
	{
		sharedDataResource->vertexBufferResourceCreateInfo.bufferResourceCreateInfo.usage = usage;

		sharedDataResource->vertexBufferResourceCreateInfo.bufferResourceCreateInfo.memoryProperty = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	}
	else if ((usage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) == VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
	{
		sharedDataResource->uniformBufferResourceCreateInfo.bufferResourceCreateInfo.usage = usage;

		// No memory properties.
	}
	else
	{
		sharedDataResource->storageBufferResourceCreateInfo.bufferResourceCreateInfo.usage = usage;

		sharedDataResource->storageBufferResourceCreateInfo.bufferResourceCreateInfo.memoryProperty = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	}

	return true;
}

bool RenderManager::sharedDataCreate(uint64_t& sharedDataHandle)
{
	SharedDataResource* sharedDataResource = getSharedData(++handles);

	if (sharedDataResource->created)
	{
		return false;
	}

	sharedDataResource->created = true;
	sharedDataHandle = handles;

	return true;
}

bool RenderManager::textureCreate(uint64_t& textureHandle)
{
	TextureDataResource* textureDataResource = getTexture(++handles);

	if (textureDataResource->created)
	{
		return false;
	}

	textureDataResource->created = true;
	textureHandle = handles;

	return true;
}

bool RenderManager::materialCreate(uint64_t& materialHandle)
{
	MaterialResource* materialResource = getMaterial(++handles);

	if (materialResource->created)
	{
		return false;
	}

	materialResource->created = true;
	materialHandle = handles;

	return true;
}

bool RenderManager::geometryCreate(uint64_t& geometryHandle)
{
	GeometryResource* geometryResource = getGeometry(++handles);

	if (geometryResource->created)
	{
		return false;
	}

	geometryResource->created = true;
	geometryHandle = handles;

	return true;
}

bool RenderManager::geometryModelCreate(uint64_t& geometryModelHandle)
{
	GeometryModelResource* geometryModelResource = getGeometryModel(++handles);

	if (geometryModelResource->created)
	{
		return false;
	}

	geometryModelResource->created = true;
	geometryModelHandle = handles;

	return true;
}

bool RenderManager::groupCreate(uint64_t& groupHandle)
{
	GroupResource* groupResource = getGroup(++handles);

	if (groupResource->created)
	{
		return false;
	}

	groupResource->created = true;
	groupHandle = handles;

	return true;
}

bool RenderManager::instanceCreate(uint64_t& instanceHandle)
{
	InstanceResource* instanceResource = getInstance(++handles);

	if (instanceResource->created)
	{
		return false;
	}

	instanceResource->created = true;
	instanceHandle = handles;

	return true;
}

bool RenderManager::lightCreate(uint64_t& lightHandle)
{
	LightResource* lightResource = getLight(++handles);

	if (lightResource->created)
	{
		return false;
	}

	lightResource->created = true;
	lightHandle = handles;

	return true;
}

bool RenderManager::cameraCreate(uint64_t& cameraHandle)
{
	CameraResource* cameraResource = getCamera(++handles);

	if (cameraResource->created)
	{
		return false;
	}

	cameraResource->created = true;
	cameraHandle = handles;

	return true;
}

bool RenderManager::worldCreate()
{
	WorldResource* worldResource = getWorld();

	if (worldResource->created)
	{
		return false;
	}

	worldResource->created = true;

	return true;
}

bool RenderManager::sharedDataCreateVertexBuffer(uint64_t sharedDataHandle, VkDeviceSize size, const void* data)
{
	return sharedDataSetData(sharedDataHandle, size, data, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
}

bool RenderManager::sharedDataCreateIndexBuffer(uint64_t sharedDataHandle, VkDeviceSize size, const void* data)
{
	return sharedDataSetData(sharedDataHandle, size, data, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
}

bool RenderManager::sharedDataCreateUniformBuffer(uint64_t sharedDataHandle, VkDeviceSize size, const void* data)
{
	return sharedDataSetData(sharedDataHandle, size, data, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
}

bool RenderManager::sharedDataCreateDynamicUniformBuffer(uint64_t sharedDataHandle, VkDeviceSize size, const void* data)
{
	VkDeviceSize alignedSize = 0;
	VkDeviceSize unalignedSize = size;
	VkDeviceSize alignment = physicalDeviceProperties.limits.minUniformBufferOffsetAlignment;

	if (!HelperVulkan::getAligenedSize(alignedSize, unalignedSize, alignment))
	{
		return false;
	}

	std::vector<uint8_t> frameData(alignedSize * frames);
	for (uint32_t i = 0; i < frames; i++)
	{
		memcpy(&frameData[i * alignedSize], data, size);
	}

	return sharedDataCreateUniformBuffer(sharedDataHandle, frameData.size(), frameData.data());
}

bool RenderManager::sharedDataCreateStorageBuffer(uint64_t sharedDataHandle, VkDeviceSize size, const void* data)
{
	return sharedDataSetData(sharedDataHandle, size, data, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
}

bool RenderManager::textureSetParameters(uint64_t textureHandle, const TextureResourceCreateInfo& textureResourceCreateInfo)
{
	TextureDataResource* textureDataResource = getTexture(textureHandle);

	if (!textureDataResource->created || textureDataResource->finalized)
	{
		return false;
	}

	textureDataResource->textureResourceCreateInfo = textureResourceCreateInfo;

	return true;
}

bool RenderManager::materialSetParameters(uint64_t materialHandle, const MaterialParameters& materialParameters)
{
	MaterialResource* materialResource = getMaterial(materialHandle);

	if (!materialResource->created || materialResource->finalized)
	{
		return false;
	}

	materialResource->materialParameters = materialParameters;

	return true;
}

bool RenderManager::materialSetTexture(uint64_t materialHandle, uint64_t textureHandle, const std::string& description, uint32_t texCoord)
{
	MaterialResource* materialResource = getMaterial(materialHandle);

	if (!materialResource->created || materialResource->finalized)
	{
		return false;
	}

	uint32_t binding = 0;
	if (materialResource->descriptorSetLayoutBindings.size() > 0)
	{
		binding = materialResource->descriptorSetLayoutBindings.back().binding + 1;
	}

	TextureDataResource* textureResource = getTexture(textureHandle);

	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
	descriptorSetLayoutBinding.binding = binding;
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

	materialResource->macros[description + "_TEXTURE"] = "";
	materialResource->macros[description + "_BINDING"] = std::to_string(binding);
	materialResource->macros[description + "_TEXCOORD"] = HelperShader::getTexCoord(texCoord);

	return true;
}

bool RenderManager::geometrySetAttribute(uint64_t geometryHandle, uint64_t sharedDataHandle, const std::string& description, uint32_t count, VkFormat format, uint32_t stride, VkDeviceSize offset, VkDeviceSize range)
{
	GeometryResource* geometryResource = getGeometry(geometryHandle);

	if (!geometryResource->created || geometryResource->finalized)
	{
		return false;
	}

	uint32_t typeCount = 0;
	if (!HelperVulkan::getTypeCount(typeCount, format))
	{
		return false;
	}

	int32_t attributeIndex = static_cast<int32_t>(geometryResource->vertexBuffers.size());

	if (description == "POSITION")
	{
		if (typeCount == 3)
		{
			geometryResource->macros[description + "_VEC3"] = "";
		}
		else
		{
			return false;
		}
	}
	else if (description == "NORMAL")
	{
		if (typeCount == 3)
		{
			geometryResource->macros[description + "_VEC3"] = "";
		}
		else
		{
			return false;
		}
	}
	else if (description == "TANGENT")
	{
		if (typeCount == 4)
		{
			geometryResource->macros[description + "_VEC4"] = "";
		}
		else
		{
			return false;
		}
	}
	else if (description == "TEXCOORD_0")
	{
		if (typeCount == 2)
		{
			geometryResource->macros[description + "_VEC2"] = "";
		}
		else
		{
			return false;
		}
	}
	else if (description == "TEXCOORD_1")
	{
		if (typeCount == 2)
		{
			geometryResource->macros[description + "_VEC2"] = "";
		}
		else
		{
			return false;
		}
	}
	else if (description == "COLOR_0")
	{
		if (typeCount == 3)
		{
			geometryResource->macros[description + "_VEC3"] = "";
		}
		else if (typeCount == 4)
		{
			geometryResource->macros[description + "_VEC4"] = "";
		}
		else
		{
			return false;
		}
	}
	else if (description == "JOINTS_0")
	{
		if (typeCount == 4)
		{
			geometryResource->macros[description + "_VEC4"] = "";
		}
		else
		{
			return false;
		}
	}
	else if (description == "JOINTS_1")
	{
		if (typeCount == 4)
		{
			geometryResource->macros[description + "_VEC4"] = "";
		}
		else
		{
			return false;
		}
	}
	else if (description == "WEIGHTS_0")
	{
		if (typeCount == 4)
		{
			geometryResource->macros[description + "_VEC4"] = "";
		}
		else
		{
			return false;
		}
	}
	else if (description == "WEIGHTS_1")
	{
		if (typeCount == 4)
		{
			geometryResource->macros[description + "_VEC4"] = "";
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	geometryResource->macros[description + "_LOC"] = std::to_string(attributeIndex);

	geometryResource->vertexInputBindingDescriptions.resize(attributeIndex + 1);
	geometryResource->vertexInputBindingDescriptions[attributeIndex].binding = attributeIndex;
	geometryResource->vertexInputBindingDescriptions[attributeIndex].stride = stride;
	geometryResource->vertexInputBindingDescriptions[attributeIndex].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	geometryResource->vertexInputAttributeDescriptions.resize(attributeIndex + 1);
	geometryResource->vertexInputAttributeDescriptions[attributeIndex].binding = attributeIndex;
	geometryResource->vertexInputAttributeDescriptions[attributeIndex].location = attributeIndex;
	geometryResource->vertexInputAttributeDescriptions[attributeIndex].format = format;
	geometryResource->vertexInputAttributeDescriptions[attributeIndex].offset = 0;

	//

	geometryResource->vertexBuffers.resize(attributeIndex + 1);
	geometryResource->vertexBuffers[attributeIndex] = getBuffer(sharedDataHandle);

	geometryResource->vertexBuffersOffsets.resize(attributeIndex + 1);
	geometryResource->vertexBuffersOffsets[attributeIndex] = offset;

	geometryResource->vertexBuffersRanges.resize(attributeIndex + 1);
	geometryResource->vertexBuffersRanges[attributeIndex] = range;

	geometryResource->count = count;

	return true;
}

bool RenderManager::geometrySetAttribute(uint64_t geometryHandle, uint64_t sharedDataHandle, const std::string& description, uint32_t count, VkFormat format)
{
	uint32_t typeCount;
	if (!HelperVulkan::getTypeCount(typeCount, format))
	{
		return false;
	}

	uint32_t componentTypeSize;
	if (!HelperVulkan::getComponentTypeSize(componentTypeSize, format))
	{
		return false;
	}

	uint32_t stride = typeCount * componentTypeSize;
	VkDeviceSize offset = 0;
	VkDeviceSize range = count * stride;

	return geometrySetAttribute(geometryHandle, sharedDataHandle, description, count, format, stride, offset, range);
}

bool RenderManager::geometryModelSetGeometry(uint64_t geometryModelHandle, uint64_t geometryHandle)
{
	GeometryModelResource* geometryModelResource = getGeometryModel(geometryModelHandle);

	if (!geometryModelResource->created || geometryModelResource->finalized)
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

bool RenderManager::geometryModelSetPrimitiveTopology(uint64_t geometryModelHandle, uint32_t mode)
{
	GeometryModelResource* geometryModelResource = getGeometryModel(geometryModelHandle);

	if (!geometryModelResource->created || geometryModelResource->finalized)
	{
		return false;
	}

	geometryModelResource->mode = mode;

	return true;
}

bool RenderManager::geometryModelSetMaterial(uint64_t geometryModelHandle, uint64_t materialHandle)
{
	GeometryModelResource* geometryModelResource = getGeometryModel(geometryModelHandle);

	if (!geometryModelResource->created || geometryModelResource->finalized)
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

	if (!geometryModelResource->created || geometryModelResource->finalized)
	{
		return false;
	}

	geometryModelResource->verticesCount = verticesCount;

	return true;
}

bool RenderManager::geometryModelSetIndices(uint64_t geometryModelHandle, uint64_t sharedDataHandle, uint32_t indicesCount, VkIndexType indexType, uint32_t indexOffset, uint32_t indexRange)
{
	GeometryModelResource* geometryModelResource = getGeometryModel(geometryModelHandle);

	if (!geometryModelResource->created || geometryModelResource->finalized)
	{
		return false;
	}

	if (indexType == VK_INDEX_TYPE_UINT8_EXT)
	{
		SharedDataResource* sharedDataResource = getSharedData(sharedDataHandle);
		const uint8_t* byteData = reinterpret_cast<const uint8_t*>(sharedDataResource->vertexBufferResourceCreateInfo.data);

		std::vector<uint16_t> newIndices(indicesCount);
		for (size_t i = 0; i < newIndices.size(); i++)
		{
			newIndices[i] = static_cast<uint16_t>(byteData[i]);
		}

		//

		indexType = VK_INDEX_TYPE_UINT16;
		indexOffset = 0;
		indexRange = 2 * indexRange;

		if (!sharedDataCreate(sharedDataHandle))
		{
			return false;
		}

		if (!sharedDataCreateIndexBuffer(sharedDataHandle, newIndices.size() * sizeof(uint16_t), newIndices.data()))
		{
			return false;
		}

		if (!sharedDataFinalize(sharedDataHandle))
		{
			return false;
		}
	}

	geometryModelResource->indicesCount = indicesCount;
	geometryModelResource->indexType = indexType;
	geometryModelResource->indexBuffer = getBuffer(sharedDataHandle);
	geometryModelResource->indexOffset = indexOffset;
	geometryModelResource->indexRange = indexRange;

	geometryModelResource->indexHandle = sharedDataHandle;

	return true;
}

bool RenderManager::geometryModelSetTarget(uint64_t geometryModelHandle, uint64_t sharedDataHandle, const std::string& targetName)
{
	GeometryModelResource* geometryModelResource = getGeometryModel(geometryModelHandle);

	if (!geometryModelResource->created || geometryModelResource->finalized)
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
		geometryModelResource->targetPositionHandle = sharedDataHandle;
	}
	else if (targetName == "NORMAL")
	{
		geometryModelResource->targetNormalHandle = sharedDataHandle;
	}
	else if (targetName == "TANGENT")
	{
		geometryModelResource->targetTangentHandle = sharedDataHandle;
	}
	else
	{
		return false;
	}

	geometryModelResource->macros["HAS_TARGET_" + targetName] = "";

	return true;
}

bool RenderManager::geometryModelSetTargetsCount(uint64_t geometryModelHandle, uint32_t targetsCount)
{
	GeometryModelResource* geometryModelResource = getGeometryModel(geometryModelHandle);

	if (!geometryModelResource->created || geometryModelResource->finalized)
	{
		return false;
	}

	geometryModelResource->targetsCount = targetsCount;

	uint32_t packedTargetsCount = targetsCount / 4;
	if (targetsCount % 4 != 0)
	{
		packedTargetsCount++;
	}

	geometryModelResource->macros["TARGETS_COUNT"] = std::to_string(packedTargetsCount);

	return true;
}

bool RenderManager::geometryModelSetCullMode(uint64_t geometryModelHandle, VkCullModeFlags cullMode)
{
	GeometryModelResource* geometryModelResource = getGeometryModel(geometryModelHandle);

	if (!geometryModelResource->created || geometryModelResource->finalized)
	{
		return false;
	}

	geometryModelResource->cullMode = cullMode;

	return true;
}

bool RenderManager::groupAddGeometryModel(uint64_t groupHandle, uint64_t geometryModelHandle)
{
	GroupResource* groupResource = getGroup(groupHandle);

	if (!groupResource->created || groupResource->finalized)
	{
		return false;
	}

	groupResource->geometryModelHandles.push_back(geometryModelHandle);

	return true;
}

bool RenderManager::instanceSetWorldMatrix(uint64_t instanceHandle, const glm::mat4& worldMatrix)
{
	InstanceResource* instanceResource = getInstance(instanceHandle);

	if (!instanceResource->created || instanceResource->finalized)
	{
		return false;
	}

	instanceResource->worldMatrix = worldMatrix;

	return true;
}

bool RenderManager::instanceSetGroup(uint64_t instanceHandle, uint64_t groupHandle)
{
	InstanceResource* instanceResource = getInstance(instanceHandle);

	if (!instanceResource->created || instanceResource->finalized)
	{
		return false;
	}

	instanceResource->groupHandle = groupHandle;

	return true;
}

bool RenderManager::instanceSetWeights(uint64_t instanceHandle, uint64_t sharedDataHandle)
{
	InstanceResource* instanceResource = getInstance(instanceHandle);

	if (!instanceResource->created || instanceResource->finalized)
	{
		return false;
	}

	SharedDataResource* sharedDataResource = getSharedData(sharedDataHandle);

	if (!sharedDataResource->finalized)
	{
		return false;
	}

	instanceResource->weightsHandle = sharedDataHandle;

	return true;
}

bool RenderManager::instanceSetJointMatrices(uint64_t instanceHandle, uint64_t sharedDataHandle, uint32_t jointMatricesCount)
{
	InstanceResource* instanceResource = getInstance(instanceHandle);

	if (!instanceResource->created || instanceResource->finalized)
	{
		return false;
	}

	SharedDataResource* sharedDataResource = getSharedData(sharedDataHandle);

	if (!sharedDataResource->finalized)
	{
		return false;
	}

	instanceResource->jointMatricesHandle = sharedDataHandle;
	instanceResource->jointMatricesCount = jointMatricesCount;

	return true;
}



bool RenderManager::lightSetEnvironment(uint64_t lightHandle, const std::string& environment)
{
	LightResource* lightResource = getLight(lightHandle);

	if (!lightResource->created || lightResource->finalized)
	{
		return false;
	}

	lightResource->environment = environment;

	return true;
}

bool RenderManager::worldAddInstance(uint64_t instanceHandle)
{
	WorldResource* worldResource = getWorld();

	if (!worldResource->created || worldResource->finalized)
	{
		return false;
	}

	worldResource->instanceHandles.push_back(instanceHandle);

	return true;
}

bool RenderManager::worldSetLight(uint64_t lightHandle)
{
	WorldResource* worldResource = getWorld();

	if (!worldResource->created || worldResource->finalized)
	{
		return false;
	}

	worldResource->lightHandle = lightHandle;

	return true;
}

bool RenderManager::worldSetCamera(uint64_t cameraHandle)
{
	WorldResource* worldResource = getWorld();

	if (!worldResource->created || worldResource->finalized)
	{
		return false;
	}

	worldResource->cameraHandle = cameraHandle;

	return true;
}

bool RenderManager::sharedDataFinalize(uint64_t sharedDataHandle)
{
	SharedDataResource* sharedDataResource = getSharedData(sharedDataHandle);

	if (!sharedDataResource->created || sharedDataResource->finalized)
	{
		return false;
	}

	VkBufferUsageFlags usage = sharedDataResource->usage;

	if (((usage & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) == VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) || ((usage & VK_BUFFER_USAGE_INDEX_BUFFER_BIT) == VK_BUFFER_USAGE_INDEX_BUFFER_BIT))
	{
		if (!VulkanResource::createVertexBufferResource(physicalDevice, device, queue, commandPool, sharedDataResource->vertexBufferResource, sharedDataResource->vertexBufferResourceCreateInfo))
		{
			return false;
		}
	}
	else if ((usage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) == VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
	{
		if (!VulkanResource::createUniformBufferResource(physicalDevice, device, sharedDataResource->uniformBufferResource, sharedDataResource->uniformBufferResourceCreateInfo))
		{
			return false;
		}
	}
	else
	{
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

	if (!textureDataResource->created || textureDataResource->finalized)
	{
		return false;
	}

	if (!VulkanResource::createTextureResource(physicalDevice, device, queue, commandPool, textureDataResource->textureResource, textureDataResource->textureResourceCreateInfo))
	{
		return false;
	}

	//

	WorldResource* worldResource = getWorld();

	textureDataResource->textureIndex = static_cast<int32_t>(worldResource->descriptorImageInfoTextures.size());

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

	if (!materialResource->created || materialResource->finalized)
	{
		return false;
	}

	//

	UniformBufferResourceCreateInfo uniformBufferResourceCreateInfo = {};

	uniformBufferResourceCreateInfo.bufferResourceCreateInfo.size = sizeof(MaterialParameters);
	uniformBufferResourceCreateInfo.bufferResourceCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

	if (!VulkanResource::createUniformBufferResource(physicalDevice, device, materialResource->uniformBufferResource, uniformBufferResourceCreateInfo))
	{
		return false;
	}

	if (!VulkanResource::copyHostToDevice(device, materialResource->uniformBufferResource.bufferResource, &materialResource->materialParameters, sizeof(materialResource->materialParameters)))
	{
		return false;
	}

	//

	uint32_t binding = 0;
	if (materialResource->descriptorSetLayoutBindings.size() > 0)
	{
		binding = materialResource->descriptorSetLayoutBindings.back().binding + 1;
	}

	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
	descriptorSetLayoutBinding = {};
	descriptorSetLayoutBinding.binding = binding;
	descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorSetLayoutBinding.descriptorCount = 1;
	descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	materialResource->descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

	VkDescriptorBufferInfo descriptorBufferInfo = {};
	descriptorBufferInfo.buffer = materialResource->uniformBufferResource.bufferResource.buffer;
	descriptorBufferInfo.offset = 0;
	descriptorBufferInfo.range = sizeof(MaterialParameters);
	materialResource->descriptorBufferInfos.push_back(descriptorBufferInfo);

	materialResource->macros["UNIFORMBUFFER_BINDING"] = std::to_string(binding);

	binding++;

	materialResource->finalized = true;

	return true;
}

bool RenderManager::geometryFinalize(uint64_t geometryHandle)
{
	GeometryResource* geometryResource = getGeometry(geometryHandle);

	if (!geometryResource->created || geometryResource->finalized)
	{
		return false;
	}

	geometryResource->finalized = true;

	return true;
}

bool RenderManager::geometryModelFinalize(uint64_t geometryModelHandle)
{
	GeometryModelResource* geometryModelResource = getGeometryModel(geometryModelHandle);

	if (!geometryModelResource->created || geometryModelResource->finalized)
	{
		return false;
	}

	bool convert = false;

	switch (geometryModelResource->mode)
	{
		case 0:
			geometryModelResource->topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
			break;
		case 1:
			geometryModelResource->topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
			break;
		case 2:
			geometryModelResource->topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
			convert = true;
			break;
		case 3:
			geometryModelResource->topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
			break;
		case 4:
			geometryModelResource->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			break;
		case 5:
			geometryModelResource->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
			break;
		case 6:
			geometryModelResource->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
			break;
		default:
			return false;
	}

	if (convert)
	{
		std::vector<uint32_t> newIndices;

		if (geometryModelResource->indexType == VK_INDEX_TYPE_NONE_KHR)
		{
			for (uint32_t i = 0; i < geometryModelResource->verticesCount; i++)
			{
				newIndices.push_back(i);
			}
			newIndices.push_back(0);
		}
		else
		{
			SharedDataResource* sharedDataResource = getSharedData(geometryModelResource->indexHandle);
			if (geometryModelResource->indexType == VK_INDEX_TYPE_UINT16)
			{
				const uint16_t* shortData = reinterpret_cast<const uint16_t*>(sharedDataResource->vertexBufferResourceCreateInfo.data);

				for (uint32_t i = 0; i < geometryModelResource->indicesCount; i++)
				{
					newIndices.push_back(static_cast<uint32_t>(shortData[i]));
				}

				newIndices.push_back(newIndices[0]);
			}
			else if (geometryModelResource->indexType == VK_INDEX_TYPE_UINT32)
			{
				const uint32_t* integerData = reinterpret_cast<const uint32_t*>(sharedDataResource->vertexBufferResourceCreateInfo.data);

				for (uint32_t i = 0; i < geometryModelResource->indicesCount; i++)
				{
					newIndices.push_back(integerData[i]);
				}

				newIndices.push_back(newIndices[0]);
			}
			else
			{
				return false;
			}
		}

		uint64_t sharedDataHandle;
		if (!sharedDataCreate(sharedDataHandle))
		{
			return false;
		}

		if (!sharedDataCreateIndexBuffer(sharedDataHandle, newIndices.size() * sizeof(uint32_t), newIndices.data()))
		{
			return false;
		}

		if (!sharedDataFinalize(sharedDataHandle))
		{
			return false;
		}

		geometryModelResource->indicesCount = static_cast<uint32_t>(newIndices.size());
		geometryModelResource->indexType = VK_INDEX_TYPE_UINT32;
		geometryModelResource->indexBuffer = getBuffer(sharedDataHandle);
		geometryModelResource->indexOffset = 0;
		geometryModelResource->indexRange = newIndices.size() * sizeof(uint32_t);
	}

	geometryModelResource->finalized = true;

	return true;
}

bool RenderManager::groupFinalize(uint64_t groupHandle)
{
	GroupResource* groupResource = getGroup(groupHandle);

	if (!groupResource->created || groupResource->finalized)
	{
		return false;
	}

	groupResource->finalized = true;

	return true;
}

bool RenderManager::instanceFinalize(uint64_t instanceHandle)
{
	InstanceResource* instanceResource = getInstance(instanceHandle);

	if (!instanceResource->created || instanceResource->finalized)
	{
		return false;
	}

	//

	GroupResource* groupResource = getGroup(instanceResource->groupHandle);

	instanceResource->instanceContainers.resize(groupResource->geometryModelHandles.size());

	for (size_t geometryModelIndex = 0; geometryModelIndex < groupResource->geometryModelHandles.size(); geometryModelIndex++)
	{
		GeometryModelResource* geometryModelResource = getGeometryModel(groupResource->geometryModelHandles[geometryModelIndex]);

		//

		GeometryResource* geometryResource = getGeometry(geometryModelResource->geometryHandle);

		//

		uint32_t binding = 0;

		std::vector<VkDescriptorSetLayout> setLayouts;
		std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;

		std::vector<VkDescriptorImageInfo> descriptorImageInfos;
		std::vector<VkDescriptorBufferInfo> descriptorBufferInfos;

		if (geometryModelResource->materialHandle > 0)
		{
			MaterialResource* materialResource = getMaterial(geometryModelResource->materialHandle);

			if (materialResource->descriptorSetLayoutBindings.size() > 0)
			{
				binding = materialResource->descriptorSetLayoutBindings.back().binding + 1;
			}

			descriptorSetLayoutBindings = materialResource->descriptorSetLayoutBindings;

			descriptorImageInfos = materialResource->descriptorImageInfos;
			descriptorBufferInfos = materialResource->descriptorBufferInfos;
		}

		// Morphing

		if (geometryModelResource->targetPositionHandle != 0)
		{
			VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
			descriptorSetLayoutBinding = {};
			descriptorSetLayoutBinding.binding = binding;
			descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			descriptorSetLayoutBinding.descriptorCount = 1;
			descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

			VkDescriptorBufferInfo descriptorBufferInfo = {};
			descriptorBufferInfo.buffer = getSharedData(geometryModelResource->targetPositionHandle)->storageBufferResource.bufferResource.buffer;
			descriptorBufferInfo.offset = 0;
			descriptorBufferInfo.range = sizeof(glm::vec3) * geometryModelResource->targetsCount * geometryResource->count;
			descriptorBufferInfos.push_back(descriptorBufferInfo);

			geometryModelResource->macros["TARGET_POSITION_BINDING"] = std::to_string(binding);

			binding++;
		}

		if (geometryModelResource->targetNormalHandle != 0)
		{
			VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
			descriptorSetLayoutBinding = {};
			descriptorSetLayoutBinding.binding = binding;
			descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			descriptorSetLayoutBinding.descriptorCount = 1;
			descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

			VkDescriptorBufferInfo descriptorBufferInfo = {};
			descriptorBufferInfo.buffer = getSharedData(geometryModelResource->targetNormalHandle)->storageBufferResource.bufferResource.buffer;
			descriptorBufferInfo.offset = 0;
			descriptorBufferInfo.range = sizeof(glm::vec3) * geometryModelResource->targetsCount * geometryResource->count;
			descriptorBufferInfos.push_back(descriptorBufferInfo);

			geometryModelResource->macros["TARGET_NORMAL_BINDING"] = std::to_string(binding);

			binding++;
		}

		if (geometryModelResource->targetTangentHandle != 0)
		{
			VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
			descriptorSetLayoutBinding = {};
			descriptorSetLayoutBinding.binding = binding;
			descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			descriptorSetLayoutBinding.descriptorCount = 1;
			descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

			VkDescriptorBufferInfo descriptorBufferInfo = {};
			descriptorBufferInfo.buffer = getSharedData(geometryModelResource->targetTangentHandle)->storageBufferResource.bufferResource.buffer;
			descriptorBufferInfo.offset = 0;
			descriptorBufferInfo.range = sizeof(glm::vec3) * geometryModelResource->targetsCount * geometryResource->count;
			descriptorBufferInfos.push_back(descriptorBufferInfo);

			geometryModelResource->macros["TARGET_TANGENT_BINDING"] = std::to_string(binding);

			binding++;
		}

		if (instanceResource->weightsHandle != 0)
		{
			VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
			descriptorSetLayoutBinding = {};
			descriptorSetLayoutBinding.binding = binding;
			descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			descriptorSetLayoutBinding.descriptorCount = 1;
			descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

			VkDescriptorBufferInfo descriptorBufferInfo = {};
			descriptorBufferInfo.buffer = getSharedData(instanceResource->weightsHandle)->uniformBufferResource.bufferResource.buffer;
			descriptorBufferInfo.offset = 0;
			descriptorBufferInfo.range = sizeof(float) * geometryModelResource->targetsCount;
			descriptorBufferInfos.push_back(descriptorBufferInfo);

			geometryModelResource->macros["WEIGHTS_BINDING"] = std::to_string(binding);

			geometryModelResource->macros["HAS_WEIGHTS"] = "";

			binding++;

			//

			instanceResource->instanceContainers[geometryModelIndex].dynamicOffsets.resize(frames, 0);

			VkDeviceSize size = getSharedData(instanceResource->weightsHandle)->size / (frames * groupResource->geometryModelHandles.size());

			for (uint32_t i = 0; i < frames; i++)
			{
				instanceResource->instanceContainers[geometryModelIndex].dynamicOffsets[i] = i * size + geometryModelIndex * groupResource->geometryModelHandles.size() * size;
			}
		}

		// Skinning

		if (instanceResource->jointMatricesHandle != 0)
		{
			VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
			descriptorSetLayoutBinding = {};
			descriptorSetLayoutBinding.binding = binding;
			descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			descriptorSetLayoutBinding.descriptorCount = 1;
			descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

			VkDescriptorBufferInfo descriptorBufferInfo = {};
			descriptorBufferInfo.buffer = getSharedData(instanceResource->jointMatricesHandle)->uniformBufferResource.bufferResource.buffer;
			descriptorBufferInfo.offset = 0;
			descriptorBufferInfo.range = sizeof(glm::mat4) * instanceResource->jointMatricesCount;
			descriptorBufferInfos.push_back(descriptorBufferInfo);

			geometryModelResource->macros["JOINT_MATRICES_BINDING"] = std::to_string(binding);
			geometryModelResource->macros["JOINT_MATRICES_COUNT"] = std::to_string(instanceResource->jointMatricesCount);

			geometryModelResource->macros["HAS_JOINTS"] = "";

			binding++;

			//

			std::vector<uint32_t> dynamicOffsets = instanceResource->instanceContainers[geometryModelIndex].dynamicOffsets;
			instanceResource->instanceContainers[geometryModelIndex].dynamicOffsets.clear();

			VkDeviceSize size = getSharedData(instanceResource->jointMatricesHandle)->size / frames;

			for (uint32_t i = 0; i < frames; i++)
			{
				if (dynamicOffsets.size() > 0)
				{
					instanceResource->instanceContainers[geometryModelIndex].dynamicOffsets.push_back(dynamicOffsets[i]);
				}

				instanceResource->instanceContainers[geometryModelIndex].dynamicOffsets.push_back(i * size);
			}
		}

		// Lighting

		WorldResource* worldResource = getWorld();

		if (worldResource->lightHandle > 0)
		{
			LightResource* lightResource = getLight(worldResource->lightHandle);

			VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
			descriptorSetLayoutBinding.binding = binding;
			descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorSetLayoutBinding.descriptorCount = 1;
			descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

			VkDescriptorImageInfo descriptorImageInfo = {};
			descriptorImageInfo.sampler = lightResource->lambertian.samplerResource.sampler;
			descriptorImageInfo.imageView = lightResource->lambertian.imageViewResource.imageView;
			descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			descriptorImageInfos.push_back(descriptorImageInfo);

			geometryModelResource->macros["LAMBERTIAN_BINDING"] = std::to_string(binding);

			binding++;

			//

			descriptorSetLayoutBinding = {};
			descriptorSetLayoutBinding.binding = binding;
			descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorSetLayoutBinding.descriptorCount = 1;
			descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

			descriptorImageInfo = {};
			descriptorImageInfo.sampler = lightResource->ggx.samplerResource.sampler;
			descriptorImageInfo.imageView = lightResource->ggx.imageViewResource.imageView;
			descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			descriptorImageInfos.push_back(descriptorImageInfo);

			geometryModelResource->macros["GGX_BINDING"] = std::to_string(binding);

			binding++;

			//

			descriptorSetLayoutBinding = {};
			descriptorSetLayoutBinding.binding = binding;
			descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorSetLayoutBinding.descriptorCount = 1;
			descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

			descriptorImageInfo = {};
			descriptorImageInfo.sampler = lightResource->ggxLUT.samplerResource.sampler;
			descriptorImageInfo.imageView = lightResource->ggxLUT.imageViewResource.imageView;
			descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			descriptorImageInfos.push_back(descriptorImageInfo);

			geometryModelResource->macros["GGX_LUT_BINDING"] = std::to_string(binding);
		}

		//

		VkResult result = VK_SUCCESS;

		//

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayoutBindings.size());
		descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutBindings.data();

		result = vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &instanceResource->instanceContainers[geometryModelIndex].descriptorSetLayout);
		if (result != VK_SUCCESS)
		{
			Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

			return false;
		}

		//

		std::vector<VkDescriptorPoolSize> descriptorPoolSizes;
		descriptorPoolSizes.resize(descriptorSetLayoutBindings.size(), {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1});

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorSetLayoutBindings.size());
		descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();
		descriptorPoolCreateInfo.maxSets = 1;

		result = vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &instanceResource->instanceContainers[geometryModelIndex].descriptorPool);
		if (result != VK_SUCCESS)
		{
			Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

			return false;
		}

		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.descriptorPool = instanceResource->instanceContainers[geometryModelIndex].descriptorPool;
		descriptorSetAllocateInfo.descriptorSetCount = 1;
		descriptorSetAllocateInfo.pSetLayouts = &instanceResource->instanceContainers[geometryModelIndex].descriptorSetLayout;

		result = vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &instanceResource->instanceContainers[geometryModelIndex].descriptorSet);
		if (result != VK_SUCCESS)
		{
			Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

			return false;
		}

		uint32_t descriptorImageInfosSize = static_cast<uint32_t>(descriptorImageInfos.size());
		uint32_t descriptorBufferInfosSize = static_cast<uint32_t>(descriptorBufferInfos.size());

		//

		std::vector<VkWriteDescriptorSet> writeDescriptorSets(descriptorImageInfosSize + descriptorBufferInfosSize);

		uint32_t imageIndex = 0;
		uint32_t bufferIndex = 0;
		for (uint32_t k = 0; k < descriptorImageInfosSize + descriptorBufferInfosSize; k++)
		{
			if (descriptorSetLayoutBindings[k].descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
			{
				writeDescriptorSets[k].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				writeDescriptorSets[k].dstSet = instanceResource->instanceContainers[geometryModelIndex].descriptorSet;
				writeDescriptorSets[k].dstBinding = k;
				writeDescriptorSets[k].dstArrayElement = 0;
				writeDescriptorSets[k].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				writeDescriptorSets[k].descriptorCount = 1;
				writeDescriptorSets[k].pImageInfo = &descriptorImageInfos[imageIndex];

				imageIndex++;
			}
			else if (descriptorSetLayoutBindings[k].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
			{
				writeDescriptorSets[k].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				writeDescriptorSets[k].dstSet = instanceResource->instanceContainers[geometryModelIndex].descriptorSet;
				writeDescriptorSets[k].dstBinding = k;
				writeDescriptorSets[k].dstArrayElement = 0;
				writeDescriptorSets[k].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				writeDescriptorSets[k].descriptorCount = 1;
				writeDescriptorSets[k].pBufferInfo = &descriptorBufferInfos[bufferIndex];

				bufferIndex++;
			}
			else if (descriptorSetLayoutBindings[k].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC)
			{
				writeDescriptorSets[k].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				writeDescriptorSets[k].dstSet = instanceResource->instanceContainers[geometryModelIndex].descriptorSet;
				writeDescriptorSets[k].dstBinding = k;
				writeDescriptorSets[k].dstArrayElement = 0;
				writeDescriptorSets[k].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
				writeDescriptorSets[k].descriptorCount = 1;
				writeDescriptorSets[k].pBufferInfo = &descriptorBufferInfos[bufferIndex];

				bufferIndex++;
			}
			else if (descriptorSetLayoutBindings[k].descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
			{
				writeDescriptorSets[k].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				writeDescriptorSets[k].dstSet = instanceResource->instanceContainers[geometryModelIndex].descriptorSet;
				writeDescriptorSets[k].dstBinding = k;
				writeDescriptorSets[k].dstArrayElement = 0;
				writeDescriptorSets[k].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				writeDescriptorSets[k].descriptorCount = 1;
				writeDescriptorSets[k].pBufferInfo = &descriptorBufferInfos[bufferIndex];

				bufferIndex++;
			}
			else
			{
				return false;
			}
		}

		vkUpdateDescriptorSets(device, descriptorImageInfosSize + descriptorBufferInfosSize, writeDescriptorSets.data(), 0, nullptr);

		setLayouts.push_back(instanceResource->instanceContainers[geometryModelIndex].descriptorSetLayout);

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

		if (!VulkanResource::createShaderModule(instanceResource->instanceContainers[geometryModelIndex].vertexShaderModule, device, vertexShaderCode))
		{
			return false;
		}

		if (!VulkanResource::createShaderModule(instanceResource->instanceContainers[geometryModelIndex].fragmentShaderModule, device, fragmentShaderCode))
		{
			return false;
		}

		//

		VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo[2] = {};

		pipelineShaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pipelineShaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		pipelineShaderStageCreateInfo[0].module = instanceResource->instanceContainers[geometryModelIndex].vertexShaderModule;
		pipelineShaderStageCreateInfo[0].pName = "main";

		pipelineShaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pipelineShaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		pipelineShaderStageCreateInfo[1].module = instanceResource->instanceContainers[geometryModelIndex].fragmentShaderModule;
		pipelineShaderStageCreateInfo[1].pName = "main";

		//
		//

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
		pipelineInputAssemblyStateCreateInfo.topology = geometryModelResource->topology;

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

		VkCullModeFlags cullMode = geometryModelResource->cullMode;

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

		VkPushConstantRange pushConstantRange = {};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(UniformPushConstant);

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
		pipelineLayoutCreateInfo.pSetLayouts = setLayouts.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

		result = vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &instanceResource->instanceContainers[geometryModelIndex].pipelineLayout);
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
		graphicsPipelineCreateInfo.layout = instanceResource->instanceContainers[geometryModelIndex].pipelineLayout;
		graphicsPipelineCreateInfo.renderPass = renderPass;

		result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &instanceResource->instanceContainers[geometryModelIndex].graphicsPipeline);
		if (result != VK_SUCCESS)
		{
			Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

			return false;
		}
	}

	//

	instanceResource->finalized = true;

	return true;
}

bool RenderManager::lightFinalize(uint64_t lightHandle)
{
	LightResource* lightResource = getLight(lightHandle);

	if (!lightResource->created || lightResource->finalized)
	{
		return false;
	}

	// LAMBERTIAN

	std::string lambertianFilename = lightResource->environment + "/" + "diffuse.ktx2";
	TextureResourceCreateInfo lambertian = {};
	lambertian.samplerResourceCreateInfo.minFilter = VK_FILTER_LINEAR;
	lambertian.samplerResourceCreateInfo.magFilter = VK_FILTER_LINEAR;
	lambertian.samplerResourceCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;

	if(!ImageDataIO::open(lambertian.imageDataResources, lambertianFilename))
	{
		return false;
	}

	if (!VulkanResource::createTextureResource(physicalDevice, device, queue, commandPool, lightResource->lambertian, lambertian))
	{
		return false;
	}

	// GGX

	std::string ggxFilename = lightResource->environment + "/" + "specular.ktx2";
	TextureResourceCreateInfo ggx = {};
	ggx.samplerResourceCreateInfo.minFilter = VK_FILTER_LINEAR;
	ggx.samplerResourceCreateInfo.magFilter = VK_FILTER_LINEAR;
	ggx.samplerResourceCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

	if(!ImageDataIO::open(ggx.imageDataResources, ggxFilename))
	{
		return false;
	}

	if (!VulkanResource::createTextureResource(physicalDevice, device, queue, commandPool, lightResource->ggx, ggx))
	{
		return false;
	}

	// GGX LUT

	std::string ggxLUTFilename = "../Resources/brdf/lut_ggx.png";
	TextureResourceCreateInfo ggxLUT = {};
	ggxLUT.samplerResourceCreateInfo.minFilter = VK_FILTER_LINEAR;
	ggxLUT.samplerResourceCreateInfo.magFilter = VK_FILTER_LINEAR;
	ggxLUT.samplerResourceCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
	ggxLUT.samplerResourceCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	ggxLUT.samplerResourceCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

	if(!ImageDataIO::open(ggxLUT.imageDataResources, ggxLUTFilename))
	{
		return false;
	}

	if (!VulkanResource::createTextureResource(physicalDevice, device, queue, commandPool, lightResource->ggxLUT, ggxLUT))
	{
		return false;
	}


	lightResource->finalized = true;

	return true;
}

bool RenderManager::cameraFinalize(uint64_t cameraHandle)
{
	CameraResource* cameraResource = getCamera(cameraHandle);

	if (!cameraResource->created || cameraResource->finalized)
	{
		return false;
	}

	cameraResource->finalized = true;

	return true;
}

bool RenderManager::worldFinalize()
{
	WorldResource* worldResource = getWorld();

	if (!worldResource->created || worldResource->finalized)
	{
		return false;
	}

	//

	for (uint64_t instanceHandle : worldResource->instanceHandles)
	{
		InstanceResource* instanceResource = getInstance(instanceHandle);

		if (!instanceResource->finalized)
		{
			return false;
		}

		if (instanceResource->groupHandle == 0)
		{
			continue;
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

	if (!worldResource->created || !worldResource->finalized)
	{
		return false;
	}

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

	if (!instanceResource->created || !instanceResource->finalized)
	{
		return false;
	}

	instanceResource->worldMatrix = worldMatrix;

	return true;
}

bool RenderManager::instanceUpdateWeights(uint64_t instanceHandle, const std::vector<float>& weights, uint32_t frameIndex)
{
	InstanceResource* instanceResource = getInstance(instanceHandle);

	if (!instanceResource->created || !instanceResource->finalized)
	{
		return false;
	}

	SharedDataResource* sharedDataResource = getSharedData(instanceResource->weightsHandle);

	VkDeviceSize offset = sharedDataResource->size / frames * frameIndex;

	if (!VulkanResource::copyHostToDevice(device, sharedDataResource->uniformBufferResource.bufferResource, weights.data(), weights.size() * sizeof(float), offset))
	{
		return false;
	}

	return true;
}

bool RenderManager::instanceUpdateJointMatrices(uint64_t instanceHandle, const std::vector<glm::mat4>& jointMatrices, uint32_t frameIndex)
{
	InstanceResource* instanceResource = getInstance(instanceHandle);

	if (!instanceResource->created || !instanceResource->finalized)
	{
		return false;
	}

	SharedDataResource* sharedDataResource = getSharedData(instanceResource->jointMatricesHandle);

	VkDeviceSize offset = sharedDataResource->size / frames * frameIndex;

	if (!VulkanResource::copyHostToDevice(device, sharedDataResource->uniformBufferResource.bufferResource, jointMatrices.data(), jointMatrices.size() * sizeof(glm::mat4), offset))
	{
		return false;
	}

	return true;
}

bool RenderManager::cameraUpdateProjectionMatrix(uint64_t cameraHandle, const glm::mat4& projectionMatrix)
{
	CameraResource* cameraResource = getCamera(cameraHandle);

	if (!cameraResource->created || !cameraResource->finalized)
	{
		return false;
	}

	cameraResource->projection = projectionMatrix;

	WorldResource* worldResource = getWorld();

	worldResource->viewProjection.projection = projectionMatrix;

	return true;
}

bool RenderManager::cameraUpdateViewMatrix(uint64_t cameraHandle, const glm::mat4& viewMatrix)
{
	CameraResource* cameraResource = getCamera(cameraHandle);

	if (!cameraResource->created || !cameraResource->finalized)
	{
		return false;
	}

	cameraResource->view = viewMatrix;

	WorldResource* worldResource = getWorld();

	worldResource->viewProjection.view = viewMatrix;

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

	width = 0;
	height = 0;

	renderPass = VK_NULL_HANDLE;
	samples = VK_SAMPLE_COUNT_1_BIT;

	imageView = VK_NULL_HANDLE;

	physicalDevice = VK_NULL_HANDLE;
	device = VK_NULL_HANDLE;
	queue = VK_NULL_HANDLE;
	commandPool = VK_NULL_HANDLE;

	handles = 0;
}

void RenderManager::draw(VkCommandBuffer commandBuffer, uint32_t frameIndex, DrawMode drawMode)
{
	WorldResource* worldResource = getWorld();

	for (size_t i = 0; i < worldResource->instanceHandles.size(); i++)
	{
		InstanceResource* instanceResource = getInstance(worldResource->instanceHandles[i]);

		if (instanceResource->groupHandle == 0)
		{
			continue;
		}

		GroupResource* groupResource = getGroup(instanceResource->groupHandle);

		for (size_t geometryModelIndex = 0; geometryModelIndex < groupResource->geometryModelHandles.size(); geometryModelIndex++)
		{
			GeometryModelResource* geometryModelResource = getGeometryModel(groupResource->geometryModelHandles[geometryModelIndex]);

			GeometryResource* geometryResource = getGeometry(geometryModelResource->geometryHandle);

			MaterialResource* materialResource = getMaterial(geometryModelResource->materialHandle);

			//

			if (materialResource->materialParameters.alphaMode == 2 && drawMode == OPAQUE)
			{
				continue;
			}
			else if (materialResource->materialParameters.alphaMode != 2 && drawMode == TRANSPARENT)
			{
				continue;
			}

			//

			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, instanceResource->instanceContainers[geometryModelIndex].graphicsPipeline);

			uint32_t dynamicOffsetCount = static_cast<uint32_t>(instanceResource->instanceContainers[geometryModelIndex].dynamicOffsets.size()) / frames;
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, instanceResource->instanceContainers[geometryModelIndex].pipelineLayout, 0, 1, &instanceResource->instanceContainers[geometryModelIndex].descriptorSet, dynamicOffsetCount, &instanceResource->instanceContainers[geometryModelIndex].dynamicOffsets.data()[frameIndex * dynamicOffsetCount]);

			uint32_t offset = 0;
			vkCmdPushConstants(commandBuffer, instanceResource->instanceContainers[geometryModelIndex].pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, offset, sizeof(worldResource->viewProjection), &worldResource->viewProjection);
			offset += sizeof(worldResource->viewProjection);
			vkCmdPushConstants(commandBuffer, instanceResource->instanceContainers[geometryModelIndex].pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, offset, sizeof(instanceResource->worldMatrix), &instanceResource->worldMatrix);
			offset += sizeof(instanceResource->worldMatrix);
			vkCmdPushConstants(commandBuffer, instanceResource->instanceContainers[geometryModelIndex].pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, offset, sizeof(geometryModelResource->verticesCount), &geometryResource->count);
			offset += sizeof(geometryResource->count);
			vkCmdPushConstants(commandBuffer, instanceResource->instanceContainers[geometryModelIndex].pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, offset, sizeof(geometryModelResource->targetsCount), &geometryModelResource->targetsCount);
			offset += sizeof(geometryModelResource->targetsCount);

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
